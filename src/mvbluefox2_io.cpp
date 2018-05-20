#include "mvbluefox2_io.h"

static bool s_boTerminated = false;

mvbluefox2_io::mvbluefox2_io()
{
#ifdef MALLOC_TRACE
    mtrace();
#endif // MALLOC_TRACE
    m_pDev = NULL;
}

mvbluefox2_io::~mvbluefox2_io()
{
}

bool mvbluefox2_io::init(const char *serial)
{
    string deviceSerial = serial;
    m_pDev = m_devMgr.getDeviceBySerial(deviceSerial);
    if (m_pDev)
    {
        // if this device offers the 'GenICam' interface switch it on, as this will
        // allow are better control over GenICam compliant devices
        conditionalSetProperty(m_pDev->interfaceLayout, dilGenICam);
        // if this device offers a user defined acquisition start/stop behaviour
        // enable it as this allows finer control about the streaming behaviour
        conditionalSetProperty(m_pDev->acquisitionStartStopBehaviour, assbUser);
        m_pDev->open();
        return true;
    }
    else
    {
        return false;
    }
}

void mvbluefox2_io::setExposure(int v)
{
    CameraSettingsBlueFOX setting(m_pDev);
    setting.expose_us.write(v);    
}

void mvbluefox2_io::setForcetoGray()
{
    ImageProcessing ip(m_pDev);
    if (ip.colorProcessing.isValid())
    {
        ip.colorProcessing.write(cpmRaw);
    }
}

void mvbluefox2_io::setDefaultRequestCount(int dc)
{
    if (m_pDev)
        m_pDev->defaultRequestCount.write(dc);
}

void mvbluefox2_io::startCapture()
{
    m_captureThread = std::thread(&mvbluefox2_io::acquisition, this);
}

void mvbluefox2_io::raw2cv(const Request *pRequest, cv::Mat &img)
{
    int openCVDataType = CV_8UC1;
    switch (pRequest->imagePixelFormat.read())
    {
    case ibpfMono8:
        openCVDataType = CV_8UC1;
        break;
    case ibpfMono10:
    case ibpfMono12:
    case ibpfMono14:
    case ibpfMono16:
        openCVDataType = CV_16UC1;
        break;
    case ibpfMono32:
        openCVDataType = CV_32SC1;
        break;
    case ibpfBGR888Packed:
    case ibpfRGB888Packed:
        openCVDataType = CV_8UC3;
        break;
    case ibpfRGBx888Packed:
        openCVDataType = CV_8UC4;
        break;
    case ibpfRGB101010Packed:
    case ibpfRGB121212Packed:
    case ibpfRGB141414Packed:
    case ibpfRGB161616Packed:
        openCVDataType = CV_16UC3;
        break;
    case ibpfMono12Packed_V1:
    case ibpfMono12Packed_V2:
    case ibpfBGR101010Packed_V2:
    case ibpfRGB888Planar:
    case ibpfRGBx888Planar:
    case ibpfYUV422Packed:
    case ibpfYUV422_10Packed:
    case ibpfYUV422_UYVYPacked:
    case ibpfYUV422_UYVY_10Packed:
    case ibpfYUV422Planar:
    case ibpfYUV444Packed:
    case ibpfYUV444_10Packed:
    case ibpfYUV444_UYVPacked:
    case ibpfYUV444_UYV_10Packed:
    case ibpfYUV444Planar:
    case ibpfYUV411_UYYVYY_Packed:
        cout << "ERROR! Don't know how to render this pixel format (" << pRequest->imagePixelFormat.readS() << ") in OpenCV! Select another one e.g. by writing to mvIMPACT::acquire::ImageDestination::pixelFormat!" << endl;
        exit(42);
        break;
    }
    cv::Mat openCVImage(cv::Size(pRequest->imageWidth.read(), pRequest->imageHeight.read()), openCVDataType, pRequest->imageData.read(), pRequest->imageLinePitch.read());
    ImageData im;
    im.m_img = openCVImage;
    std::chrono::high_resolution_clock::time_point p = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(p.time_since_epoch());
    im.m_timestamp = ms.count();
    m_data.push_back(im);
}

void mvbluefox2_io::acquisition()
{
    cout << " == " << __FUNCTION__ << " - establish access to the statistic properties...." << endl;
    // establish access to the statistic properties
    Statistics statistics(m_pDev);
    cout << " == " << __FUNCTION__ << " - create an interface to the device found...." << endl;
    // create an interface to the device found
    FunctionInterface fi(m_pDev);

    // If this is color sensor, we will NOT convert the Bayer data into a RGB image as this
    // will cost a lot of time on an embedded system
    // ImageProcessing ip(m_pDev);
    // if (ip.colorProcessing.isValid())
    // {
    //     ip.colorProcessing.write(cpmRaw);
    // }

    // SystemSettings ss(m_pDev);
    // Pre-fill the capture queue with ALL buffers currently available. In case the acquisition engine is operated
    // manually, buffers can only be queued when they have been queued before the acquisition engine is started as well.
    // Even though there can be more than 1, for this sample we will work with the default capture queue
    int requestResult = DMR_NO_ERROR;
    int requestCount = 0;

    TDMR_ERROR result = DMR_NO_ERROR;
    while( ( result = static_cast<TDMR_ERROR>( fi.imageRequestSingle() ) ) == DMR_NO_ERROR ) {};
    if( result != DEV_NO_FREE_REQUEST_AVAILABLE )
    {
        cout << "'FunctionInterface.imageRequestSingle' returned with an unexpected result: " << result
             << "(" << ImpactAcquireException::getErrorCodeAsString( result ) << ")" << endl;
    }


    manuallyStartAcquisitionIfNeeded(m_pDev, fi);
    // run thread loop
    const Request *pRequest = 0;
    const unsigned int timeout_ms = 8000; // USB 1.1 on an embedded system needs a large timeout for the first image
    int requestNr = -1;
    unsigned int cnt = 0;
    bool boError = false;
    while (!boError)
    {
        // wait for results from the default capture queue
        requestNr = fi.imageRequestWaitFor(timeout_ms);
        if (fi.isRequestNrValid(requestNr))
        {
            pRequest = fi.getRequest(requestNr);
            if (pRequest->isOK())
            {
                ++cnt;

                // here we can display some statistical information every 100th image
                if (cnt % 10 == 0)
                {
                    cout << cnt << ": Info from " << m_pDev->serial.read()
                         << ": " << statistics.framesPerSecond.name() << ": " << statistics.framesPerSecond.readS()
                         << ", " << statistics.errorCount.name() << ": " << statistics.errorCount.readS()
                         << ", " << statistics.captureTime_s.name() << ": " << statistics.captureTime_s.readS() << " Image count: " << cnt
                         << " (dimensions: " << pRequest->imageWidth.read() << "x" << pRequest->imageHeight.read() << ", format: " << pRequest->imagePixelFormat.readS();
                    cout << "), line pitch: " << pRequest->imageLinePitch.read() << endl;
                    
                }
                cv::Mat img;
                raw2cv(pRequest, img);
            }
            else
            {
                cout << "*** Error: request not OK, result: " << pRequest->requestResult << endl;
                boError = false;
            }

            // this image has been displayed thus the buffer is no longer needed...
            fi.imageRequestUnlock(requestNr);
            // send a new image request into the capture queue
            fi.imageRequestSingle();
        }
        else
        {
            // If the error code is -2119(DEV_WAIT_FOR_REQUEST_FAILED), the documentation will provide
            // additional information under TDMR_ERROR in the interface reference
            cout << "imageRequestWaitFor failed (" << requestNr << ", " << ImpactAcquireException::getErrorCodeAsString(requestNr) << ")"
                 << ", timeout value too small?" << endl;
            boError = false;
        }

        if (waitForInput(0, STDOUT_FILENO) != 0)
        {
            cout << " == " << __FUNCTION__ << " finished by user - " << endl;
            break;
        }
    }

    if (boError)
    {
        cout << " == " << __FUNCTION__ << " finished by error - " << endl;
    }

    cout << " free resources...." << endl;
    // free resources
    fi.imageRequestReset(0, 0);
    return;
}
