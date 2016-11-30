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
#define KEY_HELP       CV_KEY_F1
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
void draw_vaticObjs(cv::Mat &img, std::vector<vatic_object> objs);

// prefix a number to string
std::string prefixNumber(int num, const std::string &str);
std::vector<std::string> prefixNumber(
    const std::vector<std::string> &str);

int jumpIndex(int cur, int jump, int max, int min, bool cycling);

cv::Rect toRect(const vatic_object &obj);

typedef enum {
    adjObj_Absolute = 0,
    adjObj_Relative = 1,
} adjObj_Method;
void adjObj(vatic_object &obj, const RECT_DIR dir,
    const cv::Point &shift, adjObj_Method method = adjObj_Absolute);

#endif

