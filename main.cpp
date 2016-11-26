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

const std::wstring foldNameXml = L"Annotations";
const std::wstring foldNameImg = L"JPEGImages";
using uiDatas = struct {
        std::vector<vatic_object> objs; // current images's all bndBoxs
        int curObj; // which bndBox in gObjs
        RECT_DIR curObjSide;
        cv::Mat img; // current image
        std::atomic<bool> drawCurObj;
        std::atomic<bool> drawCurObjSide;
        int curFrame;

        foldSetting folds;
        fileManage files;
    };

void initUiDatas(uiDatas &data)
{
    data.curObj = 0;
    data.drawCurObj = false;
    data.drawCurObjSide = false;
    data.curObjSide = RECT_NONE;
    data.curFrame = 0;
}

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

void render(const uiDatas &data, int x = -65535, int y = -65535)
{
    cv::Mat draw = data.img.clone();

    // draw all bndBox
    cv::Rect bndbox = toRect(data.objs[data.curObj]);
    draw_vaticObjs(draw, data.objs);

    if (data.drawCurObj)
    {// draw current bndBox
        cv::rectangle(draw, bndbox, cv::Scalar(200, 0, 200), 3);

        if (data.drawCurObjSide)
        {// draw current bndBox's selected side
            Lint_t line = rectLine(bndbox, data.curObjSide);
            if (data.curObjSide == RECT_CENTER)
                cv::circle(draw, line.p1, 5, cv::Scalar(255, 255, 0), -1);
            else
                cv::line(draw, line.p1, line.p2, cv::Scalar(255, 255, 0), 3);
        }
    }

    // draw filename
    std::string caption = w2mb(data.files.getBaseName(data.curFrame).c_str());
    cv::putText(draw, caption, cv::Point(5, 20), 0, 0.8, cv::Scalar(30, 200, 30), 2);

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

    uiDatas *pData = (uiDatas*)param;
    uiDatas &data = *pData;
    if (data.curObj >= 0 && data.curObj < data.objs.size())
    {// curObj is valid
        data.drawCurObj = true;
        vatic_object &targetObj = data.objs[data.curObj];
        const cv::Rect bndbox = toRect(targetObj);

        cv::Point mousePos = cv::Point(x, y);
        if (mouseRoi(bndbox).contains(mousePos))
        {// mouse in ROI of bndbox, should be handle
            data.drawCurObjSide = true;

            const RECT_DIR dir = rectDir(bndbox, mousePos);

            if (mouseLeftBtnDown &&
                (event == CV_EVENT_LBUTTONDOWN
                    || event == CV_EVENT_LBUTTONUP
                    || event == CV_EVENT_MOUSEMOVE))
            {
                adjObj(targetObj, dir, mousePos, adjObj_Absolute);
            }

            // update 
            data.curObjSide = dir;
        }
        else
            data.curObjSide = RECT_NONE;
    }

    render(data, x, y);
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

void updateData(const fileManage &fmg, int idx,
    std::vector<vatic_object> &objs, cv::Mat &img)
{
    // update objects
    objs = xml_vatic_pascal_parse(fmg.getXmlName(idx));

    // update image
    std::vector<char> imgBuf = ReadFile(fmg.getImageName(idx).c_str());
    img = cv::imdecode(imgBuf, 1);
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

static int jumpIndex(int cur, int jump, int max, int min, bool cycling)
{
    if (max <= min) throw "unexpected behavior!";
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

int main(int argc, char **argv)
{
    // choose base folder (parent of Annotations and JPEGImages)
    std::wstring baseFolder = OpenFolderDialog();

    uiDatas data;
    initUiDatas(data);

    // setup folder setting
    data.folds = foldSetting(
        baseFolder,
        foldNameXml,
        foldNameImg,
        L"deleted" + foldNameXml,
        L"deleted" + foldNameImg);
    auto & folds = data.folds;

    // check Annotations and JPEGImages both exist
    checkPath(folds.base(), folds.annotations());
    checkPath(folds.base(), folds.jpegImages());
    if ( !isDirectoryExist( folds.fullDeleteAnnotations() ) )
        createFolder(folds.fullDeleteAnnotations());
    if (!isDirectoryExist(folds.fullDeleteJpegImages()))
        createFolder(folds.fullDeleteJpegImages());

    // setup file manage
    data.files = fileManage(folds);
    data.files.init();

    auto & files = data.files;
    auto & curFrame = data.curFrame;
    auto & objs = data.objs;
    auto & curObj = data.curObj;
    auto & curObjSide = data.curObjSide;
    auto & img = data.img;

    cv::Mat dummy(cv::Size(30, 30), CV_8UC3);
    cv::imshow("img", dummy);
    cv::setMouseCallback("img", onMouse, &data);

    updateData(files, curFrame, objs, img);
    cv::imshow("img", img); cv::waitKey(1);
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
            curFrame = jumpIndex(curFrame, -1, files.size(), 0, false);
            updateData(files, curFrame, objs, img);
            curObj = jumpIndex(curObj, 0, objs.size(), 0, true);
            render(data); cv::waitKey(1);
            break;
        case KEY_NEXT_FRAME:
            curFrame = jumpIndex(curFrame, 1, files.size(), 0, false);
            updateData(files, curFrame, objs, img);
            curObj = jumpIndex(curObj, 0, objs.size(), 0, true);
            render(data); cv::waitKey(1);
            break;
        case  CV_KEY_w:
            xml_vatic_pascal_modifyObjects(
                files.getXmlName(curFrame),
                objs);
            break;
        case KEY_NEXT_OBJ:
            curObj = jumpIndex(curObj, 1, objs.size(), 0, true);
            onMouse();
            break;
        case KEY_PRVE_OBJ:
            curObj = jumpIndex(curObj, -1, objs.size(), 0, true);
            onMouse();
            break;
        case KEY_ADJ_UP:
            adjObj(objs[curObj], curObjSide, cv::Point(0, -1), adjObj_Relative);
            render(data);
            break;
        case KEY_ADJ_DOWN:
            adjObj(objs[curObj], curObjSide, cv::Point(0, 1), adjObj_Relative);
            render(data);
            break;
        case KEY_ADJ_LEFT:
            adjObj(objs[curObj], curObjSide, cv::Point(-1, 0), adjObj_Relative);
            render(data);
            break;
        case KEY_ADJ_RIGHT:
            adjObj(objs[curObj], curObjSide, cv::Point(1, 0), adjObj_Relative);
            render(data);
            break;
        default:
            printf("%x\n", key);
            break;
        }
    }

    return (1);
}