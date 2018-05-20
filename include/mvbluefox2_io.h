#ifndef MVBLUEFOX2_IO
#define MVBLUEFOX2_IO
#include "mvbluefox2_common.h"
class ImageData{
public:
	ImageData(){
		m_timestamp = 0;
	}	
	~ImageData();
public:
	cv::Mat m_img;
	int64_t m_timestamp;
};

class mvbluefox2_io
{
  public:
	mvbluefox2_io();
	~mvbluefox2_io();
	bool init(const char *serial);
	void setExposure(int v);
	void setForcetoGray();
	void setDefaultRequestCount(int dc);
	void startCapture();
	void acquisition();
	void raw2cv(const Request* pRequest, cv::Mat& img );

  public:
	deque<ImageData> m_data;
	DeviceManager m_devMgr;
	Device *m_pDev;
	std::thread m_captureThread;	
};

#endif
