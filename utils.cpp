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

void draw_vaticObjs(cv::Mat &img, std::vector<vatic_object> objs)
{
    for (auto obj : objs)
    {
        const cv::Point tl(obj.xmin, obj.ymin);
        const cv::Point br(obj.xmax, obj.ymax);
        const cv::Rect bndbox = cv::Rect(tl, br);
        cv::rectangle(img, bndbox, cv::Scalar(0, 0, 200), 2);
    }

    int i = 0;
    for (auto obj : objs)
    {
        const cv::Point tl(obj.xmin, obj.ymin - 10);
        cv::putText(img, prefixNumber(i, obj.name), tl,
            0, 0.8, cv::Scalar(0, 0, 200), 2);
        i++;
    }
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