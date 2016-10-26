#ifndef _OPENCVLIB_
#define _OPENCVLIB_
// 1. you should set the path of openCV's include path in project setup option
//    that you can include the header file below
// in this case I set 
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
//#include <cvaux.h>

#define OPENCV_VERSION "249"

//#define USE_LIBV(name,ver,d) LIB_PATH##name##ver##d##".lib"
#define USE_LIBV(name,ver,d) name##ver##d##".lib"

// 3. modify the version string, in this case it's "220"
//    if in Debug mode use "d" or "" in Release mode
#ifdef _DEBUG
#define USE_LIB(name) USE_LIBV(name,OPENCV_VERSION,"d") // for debug
#else
#define USE_LIB(name) USE_LIBV(name,OPENCV_VERSION,"") // for release
#endif

// 4. which library you are used? please add below
#pragma comment(lib, USE_LIB("opencv_core"))
#pragma comment(lib, USE_LIB("opencv_highgui"))
#pragma comment(lib, USE_LIB("opencv_video"))
//#pragma comment(lib, USE_LIB("opencv_ml"))
#pragma comment(lib, USE_LIB("opencv_legacy"))
#pragma comment(lib, USE_LIB("opencv_imgproc"))
//#pragma comment(lib, USE_LIB("opencv_ts"))
#pragma comment(lib, USE_LIB("opencv_calib3d"))
#pragma comment(lib, USE_LIB("opencv_features2d"))

#undef USE_LIB
#undef USE_LIBV

// Common Problem 
// Q1. fatal error C1083: 無法開啟包含檔案: 'cv.h': No such file or directory
// A1. you don't set opencv's include path correctly

//
//#ifndef _DEBUG
//#pragma comment(lib, "videoInput.lib")
//#else
//#pragma comment(lib, "videoInputd.lib")
//#endif

#endif