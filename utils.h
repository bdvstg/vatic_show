#ifndef utils_H__
#define utils_H__

#include "opencvlib.h"
#include "cv_key_table.h"
#include "xml_vatic_pascal.h"

#define KEY_NEXT_FRAME CV_KEY_RIGHT
#define KEY_PREV_FRAME CV_KEY_LEFT
#define KEY_NEXT_OBJ   CV_KEY_UP
#define KEY_PRVE_OBJ   CV_KEY_DOWN
#define KEY_ADJ_LEFT   CV_KEY_NUMPAD_4
#define KEY_ADJ_RIGHT  CV_KEY_NUMPAD_6
#define KEY_ADJ_UP     CV_KEY_NUMPAD_8
#define KEY_ADJ_DOWN   CV_KEY_NUMPAD_2
#define KEY_END        CV_KEY_ESC
#define KEY_SAVE       CV_KEY_CtrlS
#define KEY_DEL_FRAME  CV_KEY_Delete
#define KEY_INS_FRAME  CV_KEY_Insert
#define KEY_DEL_BOX    CV_KEY_Dash
#define KEY_ADD_BOX    CV_KEY_Plus
#define KEY_HELP       CV_KEY_F1
#define KEY_PREV_CLASS CV_KEY_OpenBracket
#define KEY_NEXT_CLASS CV_KEY_ClosedBracket
typedef enum
{
    RECT_NONE = -1,
    RECT_CENTER = 0,
    RECT_UP = 1,
    RECT_DOWN = 2,
    RECT_LEFT = 3,
    RECT_RIGHT = 4,
    RECT_COUNT = 5,
} RECT_DIR;

typedef struct line_
{
    cv::Point p1;
    cv::Point p2;
} Lint_t;

// get roi from target (just extend target)
cv::Rect mouseRoi(const cv::Rect & target);

// determine which side selected by p in roi
RECT_DIR rectDir(const cv::Rect &roi, const cv::Point &p);

// get a line of roi dir side
Lint_t rectLine(const cv::Rect &roi, const RECT_DIR dir);

// draw bndBox to img
void draw_vaticObjs(cv::Mat &img, std::vector<vatic_object> objs, int curObj);

// prefix a number to string
std::string prefixNumber(int num, const std::string &str);
std::vector<std::string> prefixNumber(
    const std::vector<std::string> &str);

// give current index cur,
// jump index this time,
// max/min of index,
// how to do when out of index, cycling or fix to bounding?
// return new index after
int jumpIndex(int cur, int jump, int max, int min, bool cycling);

// convert a vatic_object to cv::Rect
cv::Rect toRect(const vatic_object &obj);

// convert many vatic_object to cv::Rect rects
std::vector<cv::Rect> toRect(const std::vector<vatic_object> &objs);

// calculate center point of cv::Rect rect
cv::Point toCenter(const cv::Rect &rect);

// convert all cv::Rect rects to it center cv::Point centers
std::vector<cv::Point> toCenter(const std::vector<cv::Rect> &rects);

// is any cv::Rect in rects contain cv::Point p
bool isContain(const std::vector<cv::Rect> &rects,
    const cv::Point &p);

// find which cv::Point in points is closest to cv::Point p, return it index
int findNearest(const std::vector<cv::Point> &points,
    const cv::Point &p);

typedef enum {
    adjObj_Absolute = 0,
    adjObj_Relative = 1,
} adjObj_Method;
void adjObj(vatic_object &obj, const RECT_DIR dir,
    const cv::Point &shift, const cv::Size &imageSize,
    adjObj_Method method = adjObj_Absolute);

int find(const std::vector<std::string> & list, std::string & target);
#endif

