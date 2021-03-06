﻿#include <atomic>
#include <chrono>
#include <map>
#include <string>
#include <thread>

#include "Application.h"
#include "singleOptionsForm.h"

#include "opencvlib.h"
#include "xml_vatic_pascal.h"
#include "FileDialogWin.h"
#include "fileManage.h"
#include "foldSetting.h"
#include "ReadFile.h"
#include "cv_key_table.h"
#include "utils.h"
#include "settings.h"


const std::wstring foldNameXml = L"Annotations";
const std::wstring foldNameImg = L"JPEGImages";
using uiDatas = struct {
        std::vector<vatic_object> objs; // current images's all bndBoxs
        int curObj; // which bndBox in gObjs
        int candidateObj;
        RECT_DIR curObjSide;
        cv::Mat img; // current image
        int curFrame;

        foldSetting folds;
        fileManage files;

        singleOptionsForm *formBndBox;
        singleOptionsForm *formClasses;
    };

void initUiDatas(uiDatas &data)
{
    data.curObj = -1;
    data.candidateObj = -1;
    data.curObjSide = RECT_NONE;
    data.curFrame = 0;
    data.formBndBox = nullptr;
    data.formClasses = nullptr;
}

void render(const uiDatas &data, int x = -65535, int y = -65535)
{
    cv::Mat draw = data.img.clone();

    // draw all bndBox
    draw_vaticObjs(draw, data.objs, data.curObj);

    if (data.objs.size() > 0)
    {// draw current bndBox
        const cv::Rect & bndbox = toRect(data.objs[data.curObj]);
        cv::rectangle(draw, bndbox, cv::Scalar(200, 0, 200), 3);

        // draw current bndBox's selected side
        Lint_t line = rectLine(bndbox, data.curObjSide);
        if (data.curObjSide == RECT_CENTER)
            cv::circle(draw, line.p1, 5, cv::Scalar(255, 255, 0), -1);
        else if (data.curObjSide != RECT_NONE)
            cv::line(draw, line.p1, line.p2, cv::Scalar(255, 255, 0), 3);
    }

    if (data.candidateObj != -1)
    {
        const auto & candiBox = toRect(data.objs[data.candidateObj]);
        cv::rectangle(draw, candiBox, cv::Scalar(200, 200, 0), 1);
    }

    // draw filename
    std::string baseName = w2mb(data.files.getBaseName(data.curFrame).c_str());
    if (!data.files.isDeleted(data.curFrame))
    {
        std::string caption = baseName;
        cv::putText(draw, caption, cv::Point(5, 20), 0, 0.8, cv::Scalar(30, 200, 30), 2);
    }
    else
    {
        std::string caption = baseName + "  DELETED !!!";
        cv::putText(draw, caption, cv::Point(5, 30), 0, 1.0, cv::Scalar(30, 30, 230), 2);
    }

    // draw mouse pointer
    cv::circle(draw, cv::Point(x, y), 3, cv::Scalar(255, 0, 0), -1);

    // show
    cv::imshow("img", draw);
}

void updateUIs(uiDatas &data)
{
    data.formBndBox->setOptions(prefixNumber(xml_vatic_get_names(data.objs)));
    data.formBndBox->setSelected(data.curObj);
    render(data);
}

void updateData(const fileManage &fmg, int idx, uiDatas &data)
{
    // update objects
    data.objs = xml_vatic_pascal_parse(fmg.getXmlName(idx));

    // update image
    std::vector<char> imgBuf = ReadFile(fmg.getImageName(idx).c_str());
    data.img = cv::imdecode(imgBuf, 1);

    // ensure curObj not out of index
    data.curObj = jumpIndex(data.curObj, 0, data.objs.size(), 0, true);
}

std::atomic<bool> mouseLeftBtnDown = false;
void onMouse(int event = -65535, int x = -65535, int y = -65535, int flags = 0, void* param = 0)
{
    if (event == CV_EVENT_LBUTTONDOWN)
        mouseLeftBtnDown = true;
    else if (event == CV_EVENT_LBUTTONUP)
        mouseLeftBtnDown = false;
    else if (event == CV_EVENT_MOUSEMOVE);
    else if (event == CV_EVENT_RBUTTONDOWN);
    else if (event == CV_EVENT_RBUTTONUP);
    else if (event == -65535);
    else
        printf("onMouse: %d\n", event);

    const cv::Point mousePos(x, y);

    uiDatas *pData = (uiDatas*)param;
    uiDatas &data = *pData;
    if (data.curObj >= 0 && data.curObj < data.objs.size())
    {// curObj is valid
        vatic_object &targetObj = data.objs[data.curObj];
        const cv::Rect bndbox = toRect(targetObj);

        if (mouseRoi(bndbox).contains(mousePos))
        {// mouse in ROI of bndbox, should be handle
            const RECT_DIR dir = rectDir(bndbox, mousePos);

            if (mouseLeftBtnDown &&
                (event == CV_EVENT_LBUTTONDOWN
                    || event == CV_EVENT_LBUTTONUP
                    || event == CV_EVENT_MOUSEMOVE))
            {
                adjObj(targetObj, dir, mousePos, data.img.size(),
                    adjObj_Absolute);
            }

            // update
            data.curObjSide = dir;
        }
        else
            data.curObjSide = RECT_NONE;
    }

    const auto rects = toRect(data.objs);
    if (isContain(rects, mousePos))
    {
        const auto centers = toCenter(rects);
        data.candidateObj = findNearest(centers, mousePos);
        if (event == CV_EVENT_RBUTTONUP)
        {
            data.curObj = data.candidateObj;
            updateUIs(data);
        }
    }
    else data.candidateObj = -1;

    render(data, x, y);
}

void showHelp()
{
    std::wstring ctx = L"";
    ctx += L"跳下一張／上一張影像：\n";
    ctx += L"　　方向鍵右 = 下一張影像\n";
    ctx += L"　　方向鍵左 = 前一張影像\n";
    ctx += L"調整框框：\n";
    ctx += L"　　選擇框框：\n";
    ctx += L"　　　　方向鍵下 = 選擇下一個框框\n";
    ctx += L"　　　　方向鍵上 = 選擇前一個框框\n";
    ctx += L"　　　　或是用滑鼠右鍵直接點選框框來選擇\n";
    ctx += L"　　　　或是在Bounding Boxs視窗中選擇\n";
    ctx += L"　　改變框框大小或是位置：\n";
    ctx += L"　　　　用滑鼠左鍵來移動目前選擇的框框或是調整框框的大小\n";
    ctx += L"　　　　右邊的數字鍵４／６／８／２可以一次只移動一個Ｐｉｘｅｌ\n";
    ctx += L"　　新增或刪除框框：\n";
    ctx += L"　　　　按加號鍵＋可以複製目前選擇的框框\n";
    ctx += L"　　　　按減號鍵－可以刪除目前選擇的框框\n";
    ctx += L"　　改變框框的類別：\n";
    ctx += L"　　　　［(中括號左邊的) = 切換成前一個類別\n";
    ctx += L"　　　　］(中括號右邊的) = 切換成下一個類別\n";
    ctx += L"　　　　或是在Classes視窗中選擇\n";
    ctx += L"　　Ctrl+S = 儲存框框的資料\n";
    ctx += L"ESC     = 結束程式\n";
    ctx += L"Delete  = 將影像/XML檔移到刪除資料夾\n";
    ctx += L"Insert  = 將影像/XML檔從刪除資料夾移回原本的資料夾\n";
    showMessageBox(L"說明", ctx);
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

std::map<int, int> jumpInt = {
    { KEY_PREV_FRAME, -1 },
    { KEY_NEXT_FRAME, 1 },
    { KEY_PRVE_OBJ, 1 },
    { KEY_NEXT_OBJ, -1 },
    { KEY_PREV_CLASS, -1},
    { KEY_NEXT_CLASS, 1 },
};

std::map<int, cv::Point> shiftPoint = {
    { KEY_ADJ_UP, cv::Point(0, -1) },
    { KEY_ADJ_DOWN, cv::Point(0, 1) },
    { KEY_ADJ_LEFT, cv::Point(-1, 0) },
    { KEY_ADJ_RIGHT, cv::Point(1, 0) },
};

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

    // so below code no need to prefix with "data."
    auto & files = data.files;
    auto & curFrame = data.curFrame;
    auto & objs = data.objs;
    auto & curObj = data.curObj;
    auto & curObjSide = data.curObjSide;
    auto & img = data.img;

    cv::Mat dummy(cv::Size(30, 30), CV_8UC3);
    cv::imshow("img", dummy);
    cv::setMouseCallback("img", onMouse, &data);

    QScopedPointer<Application> qApplication;
    if (QApplication::instance() == nullptr)
    {
        qApplication.reset(new Application(argc, argv));
    }

    QScopedPointer<singleOptionsForm> boxsForm;
    boxsForm.reset(new singleOptionsForm("Bounding Boxes", "#FFC4C4",30,270));
    boxsForm->setCallbackSelectedChange(
        [&data](int selected) -> void
        {
            data.curObj = selected;
            data.formClasses->setSelected(data.objs[selected].name);
            render(data);
        });
    boxsForm->show();
    data.formBndBox = boxsForm.data();

    std::vector<std::string> classes = tryGetClasses(data.folds.base());
    if (classes.size() <= 0)
        classes = { "motorbike", "car", "bike", "person", "bus", };

    QScopedPointer<singleOptionsForm> classesForm;
    classesForm.reset(new singleOptionsForm("Classes","#C6ADAD",25,210,265));
    classesForm->setOptions(classes);
    classesForm->setCallbackSelectedChange(
        [&data,classes](int selected) -> void
        {
            data.objs[data.curObj].name = classes[selected];
            updateUIs(data);
        });
    classesForm->show();
    data.formClasses = classesForm.data();

    updateData(files, curFrame, data);
    updateUIs(data);
    while(true)
    {
        //QApplication::processEvents();
        //cv::waitKey(1);
        int key = cv::waitKey(0);

        switch (key)
        {
        case CV_KEY_NONE:
            break;
        case KEY_END:
            exit(0);
            break;
        case KEY_PREV_FRAME: // jumpInt[KEY_PREV_FRAME] = -1
        case KEY_NEXT_FRAME: // jumpInt[KEY_NEXT_FRAME] = 1
            curFrame = jumpIndex(curFrame, jumpInt[key], files.size(), 0, false);
            updateData(files, curFrame, data);
            updateUIs(data);
            break;
        case KEY_SAVE:
            xml_vatic_pascal_modifyObjects(
                files.getXmlName(curFrame),
                objs);
            break;
        case KEY_NEXT_OBJ: // jumpInt[KEY_NEXT_OBJ] = 1
        case KEY_PRVE_OBJ: // jumpInt[KEY_PRVE_OBJ] = -1
            curObj = jumpIndex(curObj, jumpInt[key], objs.size(), 0, true);
            onMouse(-65535,-65535,-65535,0,&data);
            boxsForm->setSelected(curObj);
            break;
        case KEY_PREV_CLASS:
        case KEY_NEXT_CLASS:
            if (find(classes, data.objs[data.curObj].name) == -1)
            {
                showMessageBox(L"錯誤",
                    L"請確認類別清單與開啟xml檔的bounding box類別名稱一致！");
            }
            else
            {
                data.objs[data.curObj].name = classes[
                    jumpIndex(
                        find(classes, data.objs[data.curObj].name),
                        jumpInt[key],
                        classes.size(), 0, true)
                ];
                updateUIs(data);
            }
            break;
        case KEY_ADJ_UP:    // shiftPoint[KEY_ADJ_UP] = cv::Point(0, -1)
        case KEY_ADJ_DOWN:  // shiftPoint[KEY_ADJ_DOWN] = cv::Point(0, 1)
        case KEY_ADJ_LEFT:  // shiftPoint[KEY_ADJ_LEFT] = cv::Point(-1, 0)
        case KEY_ADJ_RIGHT: // shiftPoint[KEY_ADJ_RIGHT] = cv::Point(1, 0)
            adjObj(objs[curObj], curObjSide, shiftPoint[key], img.size(),
                adjObj_Relative);
            render(data);
            break;
        case KEY_DEL_FRAME:
            data.files.move2deleted(curFrame);
            render(data);
            break;
        case KEY_INS_FRAME:
            data.files.move2normal(curFrame);
            render(data);
            break;
        case KEY_ADD_BOX:
            if (objs.size() > 0)
                objs.push_back(objs[curObj]);
            else
                objs.push_back(
                    xml_vatic_new_object(classes[0].c_str(),150,50,150,50));
            curObj = objs.size() - 1;
            updateUIs(data);
            break;
        case KEY_DEL_BOX:
            if (objs.size() > 0)
            {
                objs.erase(objs.begin() + curObj);
                data.curObj = jumpIndex(data.curObj, 0, data.objs.size(), 0, true);
                updateUIs(data);
            }
            break;
        case KEY_HELP:
            showHelp();
            break;
        default:
            printf("%x\n", key);
            break;
        }
    }

    return (1);
}
