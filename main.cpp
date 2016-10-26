#include "opencvlib.h"
#include "xml_vatic_pascal.h"
#include "FileDialogWin.h"
#include "ReadFile.h"
#include <string>
#include <thread>
#include <chrono>
#include "cv_key_table.h"
#include "ui_utils.h"

const std::wstring path_xml = L"\\Annotations\\";
const std::wstring path_img = L"\\JPEGImages\\";
std::wstring folder;
std::vector<vatic_object> objs;
int curObj = 0;
RECT_DIR curObjSide = RECT_NONE;
cv::Mat img;

void onMouse(int event = -65535, int x = -65535, int y = -65535, int flags = 0, void* param = 0)
{//CV_EVENT_LBUTTONDOWN
    cv::Mat draw = img.clone();
    cv::circle(draw, cv::Point(x, y), 3, cv::Scalar(255, 0, 0), -1);
    if (curObj >= 0 && curObj < objs.size())
    {// curObj is valid
        const vatic_object & obj = objs[curObj];
        const cv::Point tl(obj.xmin, obj.ymin);
        const cv::Point br(obj.xmax, obj.ymax);
        const cv::Rect bndbox = cv::Rect(tl, br);
        cv::rectangle(draw, bndbox, cv::Scalar(200, 0, 200), 3);

        cv::Point mousePos = cv::Point(x, y);
        if (mouseRoi(bndbox).contains(mousePos))
        {// mouse in ROI of bndbox, should be handle
            const RECT_DIR dir = rectDir(bndbox, mousePos);
            Lint_t line = rectLine(bndbox, dir);
            if (dir == RECT_CENTER)
                cv::circle(draw, line.p1, 5, cv::Scalar(255, 255, 0), -1);
            else
                cv::line(draw, line.p1, line.p2, cv::Scalar(255, 255, 0), 3);
            curObjSide = dir;
        }
        else
            curObjSide = RECT_NONE;
    }
    cv::imshow("img", draw);
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

void update(cv::Mat &img, std::vector<vatic_object> &objs, const std::wstring & xmlfilename)
{
	std::wstring xml_name = folder + path_xml + xmlfilename;
	std::wstring imgfile = xmlfilename;
	imgfile.replace(imgfile.length() - 3, 3, L"jpg");
	std::wstring img_name = folder + path_img + imgfile;

	objs = xml_vatic_pascal_parse(xml_name);
	std::vector<char> imgBuf = ReadFile(img_name.c_str());
	img = cv::imdecode(imgBuf, 1);
	
	cv::putText(img, w2mb(xmlfilename.c_str()), cv::Point(5, 20), 0, 0.8, cv::Scalar(30, 200, 30), 2);
	draw_vaticObjs(img, objs);
}

int main(int argc, char **argv)
{
	folder = OpenFolderDialog();
	auto xmlfiles = listFiles((folder + path_xml).c_str());

	int frameNum = xmlfiles.size();
	int curFrame = 0;

	cv::Mat dummy(cv::Size(30, 30), CV_8UC3);
	cv::imshow("img", dummy);
	cv::setMouseCallback("img", onMouse);

	update(img, objs, xmlfiles[0]);
	cv::imshow("img", img); cv::waitKey(1);
	while(true)
	{
		 cv::waitKey(1);
		int key = cv::waitKey(0);
		
		if (key == -1);
		else if (key == 27)
		{
			break;
		}
		else if (key == CV_KEY_LEFT)
		{
            curFrame--;
			if (curFrame < 0) curFrame = 0;
			update(img, objs, xmlfiles[curFrame]);
			if (curObj < 0) curObj = 0;
			if (curObj >= objs.size()) curObj = objs.size() - 1;
			cv::imshow("img", img); cv::waitKey(1);
		}
		else if (key == CV_KEY_RIGHT)
		{
            curFrame++;
			if (curFrame >= frameNum) curFrame = frameNum - 1;
			update(img, objs, xmlfiles[curFrame]);
			if (curObj < 0) curObj = objs.size() - 1;
			if (curObj >= objs.size()) curObj = 0;
			cv::imshow("img", img); cv::waitKey(1);
		}
		else if (key == CV_KEY_w)
		{
			xml_vatic_pascal_modifyObjects(
				folder + path_xml + xmlfiles[curFrame],
				objs);
		}
		else if (key == CV_KEY_UP)
		{
			curObj++;
			if (curObj < 0) curObj = objs.size() - 1;
			if (curObj >= objs.size()) curObj = 0;
			onMouse();
		}
		else if (key == CV_KEY_DOWN)
		{
			curObj--;
			if (curObj < 0) curObj = objs.size() - 1;
			if (curObj >= objs.size()) curObj = 0;
			onMouse();
		}
		else
		{
			printf("%x\n", key);
		}
	}

	return (1);
}