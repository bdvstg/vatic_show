#include "opencvlib.h"
#include "xml_vatic_pascal.h"
#include "FileDialogWin.h"
#include "fileManage.h"
#include "foldSetting.h"
#include "ReadFile.h"
#include <string>
#include <thread>
#include <chrono>
#include "cv_key_table.h"
#include "ui_utils.h"
#include <atomic>

const std::wstring path_xml = L"Annotations";
const std::wstring path_img = L"JPEGImages";

std::wstring folder; // root folder (parent of Annotations and JPEGImages)
std::vector<vatic_object> gObjs; // current images's all bndBoxs
int gCurObj = 0; // which bndBox in gObjs
RECT_DIR gCurObjSide = RECT_NONE;
cv::Mat gImg; // current image
std::vector<std::wstring> gAllFilenames; // hold all files's filename
int gCurFrame; // current is nth file
std::atomic<bool> gDrawCurObj = false;
std::atomic<bool> gDrawCurObjSide = false;

void draw_vaticObjs(cv::Mat &img, std::vector<vatic_object> objs);

cv::Rect toRect(const vatic_object &obj)
{
    const cv::Point tl(obj.xmin, obj.ymin);
    const cv::Point br(obj.xmax, obj.ymax);
    return cv::Rect(tl, br);
}

typedef enum {
    adjObj_Absolute = 0,
    adjObj_Relative = 1,
} adjObj_Method;
void adjObj(vatic_object &obj, const RECT_DIR dir,
    const cv::Point &shift, adjObj_Method method = adjObj_Absolute)
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
}

void render(int x = -65535, int y = -65535)
{
    cv::Mat draw = gImg.clone();

    // draw all bndBox
    cv::Rect bndbox = toRect(gObjs[gCurObj]);
    draw_vaticObjs(draw, gObjs);

    if (gDrawCurObj)
    {// draw current bndBox
        cv::rectangle(draw, bndbox, cv::Scalar(200, 0, 200), 3);

        if (gDrawCurObjSide)
        {// draw current bndBox's selected side
            Lint_t line = rectLine(bndbox, gCurObjSide);
            if (gCurObjSide == RECT_CENTER)
                cv::circle(draw, line.p1, 5, cv::Scalar(255, 255, 0), -1);
            else
                cv::line(draw, line.p1, line.p2, cv::Scalar(255, 255, 0), 3);
        }
    }

    // draw filename
    cv::putText(draw, w2mb(gAllFilenames[gCurFrame].c_str()), cv::Point(5, 20), 0, 0.8, cv::Scalar(30, 200, 30), 2);

    // draw mouse pointer
    cv::circle(draw, cv::Point(x, y), 3, cv::Scalar(255, 0, 0), -1);

    // show
    cv::imshow("img", draw);
}

std::atomic<bool> mouseLeftBtnDown = false;
void onMouse(int event = -65535, int x = -65535, int y = -65535, int flags = 0, void* param = 0)
{
    if (event == CV_EVENT_LBUTTONDOWN)
        mouseLeftBtnDown = true;
    else if (event == CV_EVENT_LBUTTONUP)
        mouseLeftBtnDown = false;
    else if (event == CV_EVENT_MOUSEMOVE);
    else if (event == -65535);
    else
        printf("onMouse: %d\n", event);

    if (gCurObj >= 0 && gCurObj < gObjs.size())
    {// curObj is valid
        gDrawCurObj = true;
        vatic_object &targetObj = gObjs[gCurObj];
        const cv::Rect bndbox = toRect(targetObj);

        cv::Point mousePos = cv::Point(x, y);
        if (mouseRoi(bndbox).contains(mousePos))
        {// mouse in ROI of bndbox, should be handle
            gDrawCurObjSide = true;

            const RECT_DIR dir = rectDir(bndbox, mousePos);

            if (mouseLeftBtnDown &&
                (event == CV_EVENT_LBUTTONDOWN
                    || event == CV_EVENT_LBUTTONUP
                    || event == CV_EVENT_MOUSEMOVE))
            {
                adjObj(targetObj, dir, mousePos, adjObj_Absolute);
            }

            // update 
            gCurObjSide = dir;
        }
        else
            gCurObjSide = RECT_NONE;
    }

    render(x, y);
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

    for (auto obj : objs)
    {
        const cv::Point tl(obj.xmin, obj.ymin-10);
        cv::putText(img, obj.name, tl, 0, 0.8, cv::Scalar(0, 0, 200), 2);
    }
}

static std::vector<char> gImgBuf;
void updateData(const fileManage &fmg, int idx)
{
    // update objects
    gObjs = xml_vatic_pascal_parse(fmg.getXmlName(idx));

    // update image
    gImgBuf = ReadFile(fmg.getImageName(idx).c_str());
    gImg = cv::imdecode(gImgBuf, 1);
}

void checkPath(std::wstring basePath, std::wstring targetFolder)
{
    const std::wstring fullPath = basePath + L"\\" + targetFolder;
    if (!isDirectoryExist(fullPath))
    {
        showMessageBox(L"路徑不存在!!",
            L"'" + fullPath + L"' 不存在\n" +
            L"你選擇了資料夾 '" + basePath + L"'\n" +
            L"此資料夾下必須要有資料夾 '" + targetFolder + L"'");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    folder = OpenFolderDialog();

    auto folds = foldSetting(
        folder,
        L"Annotations",
        L"JPEGImages",
        L"deletedAnnotations",
        L"deletedJPEGImages");
    auto files = fileManage(folds);
    files.init();

    const std::wstring fullPathXml = folder + L"\\" + path_xml;
    const std::wstring fullPathImg = folder + L"\\" + path_img;
    checkPath(folder, path_xml);
    checkPath(folder, path_img);

    gAllFilenames = listFiles(fullPathXml.c_str());

    gCurFrame = 0;

    cv::Mat dummy(cv::Size(30, 30), CV_8UC3);
    cv::imshow("img", dummy);
    cv::setMouseCallback("img", onMouse);

    updateData(files, gCurFrame);
    cv::imshow("img", gImg); cv::waitKey(1);
    while(true)
    {
         cv::waitKey(1);
        int key = cv::waitKey(0);
        
        switch (key)
        {
        case CV_KEY_NONE:
            break;
        case KEY_END:
            exit(0);
            break;
        case KEY_PREV_FRAME:
            gCurFrame--;
            if (gCurFrame < 0) gCurFrame = 0;
            updateData(files, gCurFrame);
            if (gCurObj < 0) gCurObj = 0;
            if (gCurObj >= gObjs.size()) gCurObj = gObjs.size() - 1;
            render(); cv::waitKey(1);
            break;
        case KEY_NEXT_FRAME:
            gCurFrame++;
            if (gCurFrame >= gAllFilenames.size())
                gCurFrame = gAllFilenames.size() - 1;
            updateData(files, gCurFrame);
            if (gCurObj < 0) gCurObj = gObjs.size() - 1;
            if (gCurObj >= gObjs.size()) gCurObj = 0;
            render(); cv::waitKey(1);
            break;
        case  CV_KEY_w:
            xml_vatic_pascal_modifyObjects(
                fullPathXml + gAllFilenames[gCurFrame],
                gObjs);
            break;
        case KEY_NEXT_OBJ:
            gCurObj++;
            if (gCurObj < 0) gCurObj = gObjs.size() - 1;
            if (gCurObj >= gObjs.size()) gCurObj = 0;
            onMouse();
            break;
        case KEY_PRVE_OBJ:
            gCurObj--;
            if (gCurObj < 0) gCurObj = gObjs.size() - 1;
            if (gCurObj >= gObjs.size()) gCurObj = 0;
            onMouse();
            break;
        case KEY_ADJ_UP:
            adjObj(gObjs[gCurObj], gCurObjSide, cv::Point(0, -1), adjObj_Relative);
            render();
            break;
        case KEY_ADJ_DOWN:
            adjObj(gObjs[gCurObj], gCurObjSide, cv::Point(0, 1), adjObj_Relative);
            render();
            break;
        case KEY_ADJ_LEFT:
            adjObj(gObjs[gCurObj], gCurObjSide, cv::Point(-1, 0), adjObj_Relative);
            render();
            break;
        case KEY_ADJ_RIGHT:
            adjObj(gObjs[gCurObj], gCurObjSide, cv::Point(1, 0), adjObj_Relative);
            render();
            break;
        default:
            printf("%x\n", key);
            break;
        }
    }

    return (1);
}