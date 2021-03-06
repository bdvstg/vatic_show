#include "utils.h"
#include "opencvlib.h"
#include <math.h>

cv::Rect mouseRoi(const cv::Rect & target)
{
    cv::Point tl = target.tl();
    cv::Point br = target.br();
    tl.x -= 20;
    tl.y -= 20;
    br.x += 20;
    br.y += 20;

    cv::Rect roi = cv::Rect(tl, br);
    return roi;
}

#define DIST(a,b) (sqrt((a.x - b.x)*(a.x - b.x)) + ((a.y - b.y)*(a.y - b.y)))
RECT_DIR rectDir(const cv::Rect &roi, const cv::Point &p)
{
    const int &x = roi.x;
    const int &y = roi.y;
    const int &w = roi.width;
    const int &h = roi.height;
    const cv::Point dp[RECT_COUNT] = {
        cv::Point(x + (w / 2), y + (h / 2)), // center
        cv::Point(x + (w / 2), y), // up
        cv::Point(x + (w / 2), y + h), // down
        cv::Point(x, y + (h / 2)), // left
        cv::Point(x + w, y + (h / 2)), }; // right

    int minIdx = 0;
    double minDist = DIST(p, dp[0]);
    for (int i = 1; i < RECT_COUNT; i++)
    {
        double dist = DIST(p, dp[i]);
        if (dist < minDist)
        {
            minDist = dist;
            minIdx = i;
        }
    }
    return (RECT_DIR)minIdx;
}

Lint_t rectLine(const cv::Rect &roi, const RECT_DIR dir)
{
    cv::Point p1, p2;
    if (dir == RECT_CENTER)
    {
        p1.x = roi.x + (roi.width/2);
        p1.y = roi.y + (roi.height/2);
        p2 = p1;
    }
    else if (dir == RECT_UP)
    {
        p1 = roi.tl();
        p2.x = p1.x + roi.width;
        p2.y = p1.y;
    }
    else if (dir == RECT_DOWN)
    {
        p2 = roi.br();
        p1.x = p2.x - roi.width;
        p1.y = p2.y;
    }
    else if (dir == RECT_LEFT)
    {
        p1 = roi.tl();
        p2.x = p1.x;
        p2.y = p1.y + roi.height;
    }
    else if (dir == RECT_RIGHT)
    {
        p2 = roi.br();
        p1.x = p2.x;
        p1.y = p2.y - roi.height;
    }

    return { p1,p2 };
}

static cv::Rect get_text_rect(const std::string &text, const cv::Point &org, int fontFace, double fontScale, int thickness)
{
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(text, fontFace,
        fontScale, thickness, &baseline);
    auto rect = cv::Rect(
        org + cv::Point(0, baseline + thickness),
        org + cv::Point(textSize.width, -textSize.height));
    return rect;
}

void draw_vaticObjs(cv::Mat &img, std::vector<vatic_object> objs, int curObj)
{
    for (auto obj : objs)
    {
        const cv::Point tl(obj.xmin, obj.ymin);
        const cv::Point br(obj.xmax, obj.ymax);
        const cv::Rect bndbox = cv::Rect(tl, br);
        cv::rectangle(img, bndbox, cv::Scalar(0, 0, 200), 2);
    }

    const int fontFace = 0;
    const int thickness = 1;
    const double fontScale = 0.6;
    const auto bgColor = cv::Scalar(0, 0, 0);
    double bgAlpha = 0.7;
    const auto fontColor = cv::Scalar(200, 200, 200);
    const auto bgColorFocus = cv::Scalar(28, 165, 37);//#47b8e0 // 0xe0, 0xb8, 0x47
    const auto fontColorFocus = cv::Scalar(255,255,255); //#ff7473 //0x73, 0x74, 0xff
    cv::Mat mask = img.clone();

    std::vector<std::string> texts(objs.size());
    std::vector<cv::Point> orgs(objs.size());
    std::vector<cv::Rect> textRects(objs.size());
    for (auto i = 0; i < objs.size(); i++)
    {
        const auto &obj = objs[i];
        texts[i] = prefixNumber(i, obj.name);
        orgs[i] = cv::Point(obj.xmin, obj.ymin - 10);
        textRects[i] =
            get_text_rect(texts[i], orgs[i], fontFace, fontScale, thickness) +
            cv::Point(-5, -5) +
            cv::Size(12, 6);
        cv::rectangle(mask, textRects[i], bgColor, -1);
    }

    cv::addWeighted(mask, bgAlpha, img, 1.0 - bgAlpha, 0, img);

    for (auto i = 0; i < objs.size(); i++)
    {
        cv::putText(img, texts[i], orgs[i],
            fontFace, fontScale, fontColor, thickness);
    }

    cv::rectangle(img, textRects[curObj], bgColorFocus, -1);
    cv::putText(img, texts[curObj], orgs[curObj],
        fontFace, fontScale, fontColorFocus, thickness);
}

std::string prefixNumber(int num, const std::string &str)
{
    return std::to_string(num) + ". " + str;
}

std::vector<std::string> prefixNumber(
    const std::vector<std::string> &str)
{
    std::vector<std::string> prefixs(str.size());
    for (int i = 0; i < str.size(); i++)
    {
        prefixs[i] = prefixNumber(i, str[i]);
    }
    return prefixs;
}

int jumpIndex(int cur, int jump, int max, int min, bool cycling)
{
    if (max < min) throw "unexpected behavior!";
    if (max == min) return -1;
    jump %= (max - min);
    cur += jump;

    if (cur >= max)
        if (cycling)
            cur = min + (cur - max);
        else
            cur = max - 1;

    if (cur < min)
        if (cycling)
            cur = max - (min - cur);
        else
            cur = min;

    return cur;
}

cv::Rect toRect(const vatic_object &obj)
{
    const cv::Point tl(obj.xmin, obj.ymin);
    const cv::Point br(obj.xmax, obj.ymax);
    return cv::Rect(tl, br);
}

std::vector<cv::Rect> toRect(const std::vector<vatic_object> &objs)
{
    std::vector<cv::Rect> rects(objs.size());
    for (int i = 0; i < objs.size(); i++)
    {
        const auto & obj = objs[i];
        const cv::Point tl(obj.xmin, obj.ymin);
        const cv::Point br(obj.xmax, obj.ymax);
        rects[i] = cv::Rect(tl, br);
    }
    return rects;
}

void adjObj(vatic_object &obj, const RECT_DIR dir,
    const cv::Point &shift, const cv::Size &imageSize,
    adjObj_Method method)
{
    const int width = obj.xmax - obj.xmin;
    const int height = obj.ymax - obj.ymin;
    const int minWidth = 6;
    const int minHeight = 10;

    if (method == adjObj_Absolute)
    {
        if (dir == RECT_UP)
            obj.ymin = shift.y;
        else if (dir == RECT_DOWN)
            obj.ymax = shift.y;
        else if (dir == RECT_LEFT)
            obj.xmin = shift.x;
        else if (dir == RECT_RIGHT)
            obj.xmax = shift.x;
        else if (dir == RECT_CENTER)
        {
            obj.xmin = shift.x - (width / 2);
            obj.xmax = shift.x + (width / 2);
            obj.ymin = shift.y - (height / 2);
            obj.ymax = shift.y + (height / 2);
        }
    }
    else if (method == adjObj_Relative)
    {
        if (dir == RECT_UP)
            obj.ymin += shift.y;
        else if (dir == RECT_DOWN)
            obj.ymax += shift.y;
        else if (dir == RECT_LEFT)
            obj.xmin += shift.x;
        else if (dir == RECT_RIGHT)
            obj.xmax += shift.x;
        else if (dir == RECT_CENTER)
        {
            obj.ymin += shift.y;
            obj.ymax += shift.y;
            obj.xmin += shift.x;
            obj.xmax += shift.x;
        }
    }

    // ensure not small than min box
    if (height < minHeight)
    {
        if (dir == RECT_UP)
            obj.ymin = obj.ymax - minHeight;
        else if (dir == RECT_DOWN)
            obj.ymax = obj.ymin + minHeight;
    }
    if (width < minWidth)
    {
        if (dir == RECT_LEFT)
            obj.xmin = obj.xmax - minWidth;
        else if (dir == RECT_RIGHT)
            obj.xmax = obj.xmin + minWidth;
    }

    // filter with image roi
    const auto img = cv::Rect(cv::Point(0, 0), imageSize);
    const auto box = toRect(obj);
    const auto intersec = img & box;
    obj.xmin = intersec.tl().x;
    obj.ymin = intersec.tl().y;
    obj.xmax = intersec.br().x;
    obj.ymax = intersec.br().y;
}

cv::Point toCenter(const cv::Rect &rect)
{
    return {rect.x + (rect.width / 2), rect.y + (rect.height / 2)};
}
std::vector<cv::Point> toCenter(const std::vector<cv::Rect> &rects)
{
    std::vector<cv::Point> centers(rects.size());
    for (int i = 0; i < rects.size(); i++)
    {
        centers[i] = toCenter(rects[i]);
    }
    return centers;
}
bool isContain(const std::vector<cv::Rect> &rects,
    const cv::Point &p)
{
    bool contain = false;
    for each (auto rect in rects)
    {
        if (rect.contains(p))
        {
            contain = true;
            break;
        }
    }
    return contain;
}

using idxData = struct
{
    int index;
    const cv::Point *data;
};

int findNearest(const std::vector<cv::Point> &points,
    const cv::Point &p)
{
    if (points.empty()) return -1;
    std::vector<idxData> idxDatas(points.size());
    for (int i = 0; i < points.size(); i++)
    {
        idxDatas[i].index = i;
        idxDatas[i].data = &points[i];
    }

#define DIST(A,B) sqrt(((A).x-(B).x)*((A).x-(B).x) +\
    ((A).y-(B).y)*((A).y-(B).y))

    std::sort(idxDatas.begin(), idxDatas.end(),
        [p](const idxData & a, const idxData & b)
        {
            float da = DIST(p, *(a.data));
            float db = DIST(p, *(b.data));
            return da < db;
        });
    return idxDatas[0].index;
}

int find(const std::vector<std::string> & list, std::string & target)
{
    int matchIndex = -1; // no found
    const auto match = // find target in list, get it iterator
        std::find_if(list.begin(), list.end(),
            [target](const std::string &str)
            {
                return (str.compare(target) == 0);
            });
    if (match >= list.begin() && match < list.end())
    {
        matchIndex = match - list.begin();
    }
    return matchIndex;
}
