#include "FeatureDetector.h"
#include <opencv2/imgproc.hpp>
#include "../common/common.hpp"

#pragma warning(disable: 4244 4267 4018)
#define BIN_DIFF
bool CheckFeatures(Obj2d* obj, type_condition condition, int features_to_check)
{
	bool res = true;
	if(features_to_check & FEATURE_CHECK_WIDTH)
		if (!(condition.rect[0].width <= obj->rect.width) || !(obj->rect.width <= condition.rect[1].width))
			res = false;
	if(features_to_check & FEATURE_CHECK_HEIGHT)
		if (!(condition.rect[0].height <= obj->rect.height) || !(obj->rect.height <= condition.rect[1].height))
			res = false;
	double size_ratio = obj->r_rect.size.width / obj->r_rect.size.height;
	if(size_ratio < 1)
		size_ratio = 1 / size_ratio;
	if(features_to_check & FEATURE_CHECK_SIZE_RATIO)
		if(!(condition.size_ratio[0] <= size_ratio) || !(size_ratio <= condition.size_ratio[1]))
			res = false;
	double square_ratio =  obj->square / ((obj->r_rect.size.width+1)*(obj->r_rect.size.height+1));
	if(features_to_check & FEATURE_CHECK_SQUARE_RATIO)
		if(!(condition.square_ratio[0] <= square_ratio) || !(square_ratio <= condition.square_ratio[1]))
			res = false;
	double bounding_size_ratio = (double)obj->rect.height/obj->rect.width;
	if(features_to_check & FEATURE_CHECK_BOUNDING_SIZE_RATIO)
		if(	!(condition.bounding_size_ratio[0] <= bounding_size_ratio) ||
			!(bounding_size_ratio <= condition.bounding_size_ratio[1]))
			res = false;
	// need to add new checks
	return res;
}
void GetObj2d(Obj2d* obj)
{
	obj->rect = cv::boundingRect(obj->contours[0][0]);
	obj->r_rect = cv::minAreaRect(obj->contours[0][0]);
	cv::Mat img = cv::Mat::zeros(obj->rect.size(), CV_8UC1);
	std::vector<cv::Scalar> colors = {cv::Scalar(255), cv::Scalar(0)};
	DrawContours(obj->contours, colors, img, cv::Point(-obj->rect.x, -obj->rect.y));
	obj->square = countNonZero(img);
}
std::vector<Obj2d> FindObjects(cv::Mat img, std::vector<type_condition> conditions, std::vector<int> features_to_check, int mode, int level_limit, cv::Point offset)
{
	std::vector<Obj2d> res;
	std::vector<contour_type> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat img_1px;
	cv::copyMakeBorder(img, img_1px, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
	cv::findContours(img_1px, contours, hierarchy, mode, cv::CHAIN_APPROX_SIMPLE, offset + cv::Point(-1, -1));
	if(level_limit < 0)
		level_limit = INT_MAX;
	std::set<unsigned> banished;
	for(unsigned i = 0; i < contours.size(); i++)
	{
		if(banished.find(i) != banished.end()) // preserve from double including parent and child contours
			continue;
		if(GetContourLevel(hierarchy, i) > level_limit)
			continue;
		Obj2d temp_obj;
		temp_obj.contours = GetContours(GetContours(hierarchy, i), contours);
		GetObj2d(&temp_obj);
		if(!conditions.size() || !conditions.size())
		{
			temp_obj.tag = -1;
			res.push_back(temp_obj);
		}
		else
		for(unsigned j = 0; j < conditions.size(); j++)
			if(CheckFeatures(&temp_obj, conditions[j], features_to_check[j % features_to_check.size()]))
			{
				temp_obj.tag = j;
				res.push_back(temp_obj);
				BanishContour(banished, hierarchy, i);
				break;
			}		
	}
	return res;
}
void DrawContours(	std::vector<std::vector<contour_type>> contours,
					std::vector<cv::Scalar> colors, cv::Mat& img, 
					cv::Point offset, int level_limit)
{
	if (level_limit < 0)
		level_limit = contours.size();
	for(unsigned i = 0; i < level_limit; i++)
	{
		unsigned color_num = i % colors.size(); // cyclically switching colors
		cv::drawContours(img, contours[i], -1, colors[color_num], CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, offset);
	}
}
int GetContourLevel(std::vector<cv::Vec4i> hierarchy, unsigned contour_num)
{
	int res = 0;
	for(;hierarchy[contour_num][3] != -1; res++) // -1 means that contour hasn't parent
		contour_num = hierarchy[contour_num][3]; 
	return res;
}
std::vector<unsigned> GetChildren(std::vector<cv::Vec4i> hierarchy, unsigned contour_num)
{
	std::vector<unsigned> res;
	for(unsigned i = 0; i < hierarchy.size(); i++)
	{
		if(hierarchy[i][3] == contour_num) // [i][3] is parent of i
			res.push_back(i);
	}
	return res;
}
std::vector<std::vector<unsigned>> GetContours(std::vector<cv::Vec4i> hierarchy, unsigned contour_num)
{
	std::vector<std::vector<unsigned>> res;
	std::vector<unsigned> current_contours;
	current_contours.push_back(contour_num);
	res.push_back(current_contours);
	while(current_contours.size())
	{
		std::vector<unsigned> children_level; // all children at current level
		for(auto current: current_contours)
		{
			std::vector<unsigned> children = GetChildren(hierarchy, current);
			children_level.insert(children_level.end(), children.begin(), children.end());
		}
		if(children_level.size())
			res.push_back(children_level);
		current_contours = children_level;
	}
	return res;
}
std::vector<std::vector<contour_type>> GetContours(std::vector<std::vector<unsigned>> contours_nums, std::vector<contour_type> contours)
{
	std::vector<std::vector<contour_type>> res;
	for(auto level_nums: contours_nums)
	{
		std::vector<contour_type> level;
		for(auto contour_num: level_nums)
			level.push_back(contours[contour_num]);
		res.push_back(level);
	}
	return res;
}
void BanishContour(std::set<unsigned>& banished, std::vector<cv::Vec4i> hierarchy, unsigned contour_num)
{
	std::vector<std::vector<unsigned>> contours = GetContours(hierarchy, contour_num);
	for(auto level: contours)
		for(auto contour: level)
			banished.insert(contour);
}
void DrawRect(cv::Rect rect, cv::Mat& img, cv::Scalar color, int thickness)
{
	cv::rectangle(img, rect, color, thickness);
}
void DrawRRect(cv::RotatedRect r_rect, cv::Mat& img, cv::Scalar color, int thickness)
{
	cv::Point2f points_f[4];
	r_rect.points(points_f);
	std::vector<cv::Point> points = {points_f[0], points_f[1], points_f[2], points_f[3]};
	if(thickness < 0)
		cv::fillPoly(img, points, color);
	else
		cv::polylines(img, points, true, color, thickness);
}
cv::Mat Binarize(cv::Mat img)
{
	cv::Mat res;
	cv::Mat img_blur;
	cv::medianBlur(img, img_blur, 5);
		//SHOW_N_WAIT(img_blur);
#ifdef BIN_DIFF
	cv::Mat img_bg;
	cv::blur(img_blur, img_bg, img_blur.size()/3);
		//SHOW_N_WAIT(img_bg);
	img_blur = img_bg - img_blur;
	//	SHOW_N_WAIT(img_blur);
	cv::threshold(img_blur, res, 0, 255, cv::THRESH_OTSU);
#else
	cv::Canny(img_blur, res, 50, 150);
		//SHOW_N_WAIT(res);
	cv::morphologyEx(	res, res, cv::MORPH_CLOSE, 
						cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5))); // clean
#endif
		//SHOW_N_WAIT(res);
	return res;
}
cv::Scalar RandomColor(cv::RNG& rng)
{
	int color = (unsigned)rng;
	return cv::Scalar(color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF);
}
Obj2d RotateObj(Obj2d& obj, double angle)
{
	float diag = sqrt(pow(obj.rect.width, 2) + pow(obj.rect.height, 2));
	cv::Size field_size(diag*2, diag*2);
	cv::Point offset(0,0);
	offset -= obj.r_rect.center - diag; 
	cv::Mat obj_field(field_size, CV_8UC1, cv::Scalar(0));
	DrawContours(obj.contours, {cv::Scalar(255)}, obj_field, offset);
	cv::Point2f rot_center = obj.r_rect.center - obj.rect.tl() + diag;
	cv::Mat rot_mat = cv::getRotationMatrix2D(rot_center, angle, 1);
	cv::warpAffine(obj_field, obj_field, rot_mat, field_size);
	std::vector<Obj2d> res = FindObjects(obj_field, std::vector<type_condition>(),  std::vector<int>(), cv::RETR_EXTERNAL);
	return res[0];
}

double VSymmetry(cv::Mat img)
{
	if(img.rows < 2)
		return 0;
	int rows = img.rows/2;
	cv::Mat top_half = img.rowRange(0, rows).clone();
	cv::Mat bottom_half = img.rowRange(img.rows - rows, img.rows).clone();
	cv::flip(bottom_half, bottom_half, 0);
	cv::Mat diff;
	cv::absdiff(top_half, bottom_half, diff);
	cv::Scalar diff_sum = cv::sum(diff);
	cv::Scalar img_sum = cv::sum(img);
	return diff_sum[0]/img_sum[0]*2;
}
double HSymmetry(cv::Mat img)
{
	if(img.cols < 2)
		return 0;
	int cols = img.rows/2;
	cv::Mat left_half = img.colRange(0, cols).clone();
	cv::Mat right_half = img.colRange(img.cols - cols, img.cols).clone();
	cv::flip(right_half, right_half, 1);
	cv::Mat diff;
	cv::absdiff(left_half, right_half, diff);
	cv::Scalar diff_sum = cv::sum(diff);
	cv::Scalar img_sum = cv::sum(img);
	return diff_sum[0]/img_sum[0]*2;
}
double VHSymmetry(cv::Mat img) // VHS symmetry, lol
{
	return (VSymmetry(img) + HSymmetry(img))/2;
}