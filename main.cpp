#include "opencvlib.h"
#include "xml_vatic_pascal.h"
#include "FileDialogWin.h"
#include "ReadFile.h"
#include <string>
#include <thread>
#include <chrono>
#include "cv_key_table.h"

const std::wstring path_xml = L"\\Annotations\\";
const std::wstring path_img = L"\\JPEGImages\\";
std::wstring folder;
std::vector<vatic_object> objs;
int curObj = 0;
cv::Mat img;

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

void onMouse(int event, int x, int y, int flags, void* param)
{
	cv::Mat draw = img.clone();
	cv::circle(draw, cv::Point(x, y), 3, cv::Scalar(255, 0, 0), -1);
	if (curObj >= 0 && curObj < objs.size())
	{
		const vatic_object & obj = objs[curObj];
		const cv::Point tl(obj.xmin, obj.ymin);
		const cv::Point br(obj.xmax, obj.ymax);
		const cv::Rect bndbox = cv::Rect(tl, br);
		cv::rectangle(draw, bndbox, cv::Scalar(200, 0, 200), 2);
	}
	cv::imshow("img", draw);
}

int main(int argc, char **argv)
{
	folder = OpenFolderDialog();
	auto xmlfiles = listFiles((folder + path_xml).c_str());

	int num = xmlfiles.size();
	int i = 0;

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
			i--;
			if (i < 0) i = 0;
			update(img, objs, xmlfiles[i]);
			if (curObj < 0) curObj = 0;
			if (curObj >= objs.size()) curObj = objs.size() - 1;
			cv::imshow("img", img); cv::waitKey(1);
		}
		else if (key == CV_KEY_RIGHT)
		{
			i++;
			if (i >= num) i = num - 1;
			update(img, objs, xmlfiles[i]);
			if (curObj < 0) curObj = 0;
			if (curObj >= objs.size()) curObj = objs.size() - 1;
			cv::imshow("img", img); cv::waitKey(1);
		}
		else if (key == CV_KEY_w)
		{
			xml_vatic_pascal_modifyObjects(
				folder + path_xml + xmlfiles[i],
				objs);
		}
		else if (key == CV_KEY_UP)
		{
			curObj++;
			if (curObj < 0) curObj = 0;
			if (curObj >= objs.size()) curObj = objs.size() - 1;
			onMouse(0, 0, 0, 0, 0);
		}
		else if (key == CV_KEY_DOWN)
		{
			curObj--;
			if (curObj < 0) curObj = 0;
			if (curObj >= objs.size()) curObj = objs.size() - 1;
			onMouse(0, 0, 0, 0, 0);
		}
		else
		{
			printf("%x\n", key);
		}
	}

	return (1);
}