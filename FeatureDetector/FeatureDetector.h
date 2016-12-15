#pragma once
#include <vector>
#include <set>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

typedef std::vector<cv::Point> contour_type; 

struct Obj2d
{
	cv::Rect rect;
	cv::RotatedRect r_rect;
	std::vector<std::vector<contour_type>> contours;
	double square;
	int tag;
};

struct type_condition
{
	cv::Rect rect[2];
	cv::RotatedRect r_rect[2];
	
	double square[2]; // min max

	double square_ratio[2]; // min max
	double size_ratio[2]; // min max
	double bounding_size_ratio[2]; // min max
};

#define FEATURE_CHECK_WIDTH			0x0001
#define FEATURE_CHECK_HEIGHT		0x0002
#define FEATURE_CHECK_SIZE			(FEATURE_CHECK_WIDTH | FEATURE_CHECK_HEIGHT)
#define FEATURE_CHECK_MIN			0x0004 // min size in r_rect
#define FEATURE_CHECK_MAX			0x0008 // max size in r_rect
#define FEATURE_CHECK_MINMAX		(FEATURE_CHECK_MIN | FEATURE_CHECK_MAX)
#define FEATURE_CHECK_SQUARE		0x0010 
#define FEATURE_CHECK_SQUARE_RATIO	0x0020
#define FEATURE_CHECK_SIZE_RATIO	0x0040
#define FEATURE_CHECK_BOUNDING_SIZE_RATIO	0x0080 // height/width
#define FEATURE_CHECK_ANGLE			0x0100
#define FEATURE_CHECK_CENTER		0x0200

#define WND_NAME_TEMP	""
#define SHOW_N_WAIT(img)	cv::imshow(WND_NAME_TEMP, img); cv::waitKey(0);



bool CheckFeatures(Obj2d* obj, type_condition condition, int features_to_check);
void GetObj2d(Obj2d* obj);
std::vector<Obj2d> FindObjects(cv::Mat img, std::vector<type_condition> conditions, std::vector<int> features_to_check, int mode, int level_limit = -1, cv::Point offset = cv::Point(0, 0));
void DrawContours(	std::vector<std::vector<contour_type>> contours, 
					std::vector<cv::Scalar> colors, cv::Mat& img,
					cv::Point offset = cv::Point(), int level_limit = -1);
int GetContourLevel(std::vector<cv::Vec4i> hierarchy, unsigned contour_num);
std::vector<unsigned> GetChildren(std::vector<cv::Vec4i> hierarchy, unsigned contour_num);
std::vector<std::vector<unsigned>> GetContours(std::vector<cv::Vec4i> hierarchy, unsigned contour_num);
std::vector<std::vector<contour_type>> GetContours(std::vector<std::vector<unsigned>> contours_nums, std::vector<contour_type> contours);
void BanishContour(std::set<unsigned>& banished, std::vector<cv::Vec4i> hierarchy, unsigned contour_num);

void DrawRect(cv::Rect rect, cv::Mat& img, cv::Scalar color = cv::Scalar(255, 255, 255), int thickness = 1);
void DrawRRect(cv::RotatedRect r_rect, cv::Mat& img, cv::Scalar color = cv::Scalar(255, 255, 255), int thickness = 1);

cv::Mat Binarize(cv::Mat img);
cv::Scalar RandomColor(cv::RNG& rng);

Obj2d RotateObj(Obj2d& obj, double angle);

double VSymmetry(cv::Mat img);
double HSymmetry(cv::Mat img);
double VHSymmetry(cv::Mat img);