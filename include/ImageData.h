#include "mvbluefox2_common.h"

#ifndef IMAGEDATA
#define IMAGEDATA

class ImageData{
public:
	ImageData();
	~ImageData();
public:
	cv::Mat m_img;
	int64_t m_timestamp;

};
#endif
