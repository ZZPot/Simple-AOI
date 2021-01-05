#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>
#include "FeatureDetector/FeatureDetector.h"
#include "files/files.h"
#include <iostream>

#pragma warning(disable: 4267 4244)

using namespace cv;
using namespace cv::ml;

// we will use size ratio, square ratio and max size
#define FEATURES_COUNT	3
#define TEST_IMAGE	"1.jpg"

std::vector<float> ExtractFeatures(Obj2d obj);
Ptr<SVM> TrainModel(std::vector<std::tstring> dir_names);
void Predict(Ptr<SVM> svm, std::vector<Obj2d>& objects); // initially field tag == -1
float TestModel(Ptr<SVM> svm, std::vector<Obj2d>& objects); // initially field tag != -1

//#define TRAIN_MODEL

int main()
{
	std::vector<Scalar> rect_colors = {	Scalar(255, 0, 0), 
									Scalar(0, 0, 255),
									Scalar(0, 255, 0),
									Scalar(255, 0, 255)};
	std::vector<std::tstring> dir_names = {_T("images/0"), _T("images/1"), _T("images/2") };
	std::vector<std::tstring> obj_names = { _T("pick"), _T("screw"), _T("coin") };
	cv::String model_file = "model.xml";
	Ptr<SVM> svm;
#ifdef TRAIN_MODEL
	svm = TrainModel(dir_names);
	// save
	svm->save(model_file);
#else
	// load
	svm = SVM::load(model_file);
	Mat test_img = imread(TEST_IMAGE);
	Mat test_img_gray;
	cvtColor(test_img, test_img_gray, COLOR_BGR2GRAY);
	test_img_gray = Binarize(test_img_gray);
	std::vector<Obj2d> objects = FindObjects(test_img_gray, std::vector<type_condition>(), std::vector<int>(), RETR_EXTERNAL);

	//DrawContours(objects[0].contours, rect_colors, test_img_gray);
	//SHOW_N_WAIT(test_img_gray);

	Predict(svm, objects);
	std::vector<Scalar> colorss = {Scalar::all(255), Scalar::all(0)};
	for(auto& obj: objects)
	{
		DrawRRect(obj.r_rect, test_img, rect_colors[obj.tag]);
		putText(test_img, TcharToChar(obj_names[obj.tag].c_str(), CP_ACP), obj.r_rect.center, FONT_HERSHEY_SIMPLEX, 0.75, rect_colors[obj.tag], 1, LINE_AA);
	}
		SHOW_N_WAIT(test_img);
	imwrite("result.png", test_img);
#endif
	return 0;
}

std::vector<float> ExtractFeatures(Obj2d obj)
{
	float size_ratio = obj.r_rect.size.width / obj.r_rect.size.height;
	if(size_ratio < 1)
		size_ratio = 1 / size_ratio;
	float square_ratio = (obj.r_rect.size.width * obj.r_rect.size.width) / obj.square;
	float max_size = max(obj.r_rect.size.width, obj.r_rect.size.width);
	std::vector<float> res = {size_ratio, square_ratio, max_size};
	return res;
}

Ptr<SVM> TrainModel(std::vector<std::tstring> dir_names)
{
	std::vector<Obj2d> objects;
	for(unsigned i = 0; i < dir_names.size(); i++)
	{
		Collect files; 
		CrawlFolder(dir_names[i].c_str(), (unsigned)-1, 0, &files);
		for(auto file_name: files.file_names)
		{
			std::vector<Obj2d> objects_temp = FindObjects(
										Binarize(imread(TcharToChar(file_name.c_str(), CP_ACP), IMREAD_GRAYSCALE)),
										std::vector<type_condition>(), std::vector<int>(),
										RETR_EXTERNAL, 0);
			for(auto& obj: objects_temp)
				obj.tag = i;
			objects.insert(objects.end(), objects_temp.begin(), objects_temp.end());
		}
	}
	std::vector<float> features;
	std::vector<int> tags;
	for(auto& obj: objects)
	{
		std::vector<float> temp = ExtractFeatures(obj);
		features.insert(features.end(), temp.begin(), temp.end());
		tags.push_back(obj.tag);
	}

	// Creating cv::Mat for features and cv::Mat for tags
	Mat features_data(features.size()/FEATURES_COUNT, FEATURES_COUNT, CV_32FC1, &features[0]);
	Mat tags_data(tags.size(), 1, CV_32SC1, &tags[0]);

	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::CHI2);
	TermCriteria tc(TermCriteria::MAX_ITER, 100, 1e-6);
	svm->setTermCriteria(tc);

	svm->train(features_data, ROW_SAMPLE, tags_data);
	return svm;
}
void Predict(Ptr<SVM> svm, std::vector<Obj2d>& objects)
{
	std::vector<float> features;
	//	std::vector<float> tags; // for predict it should be float
	for(auto& obj: objects)
	{
		std::vector<float> temp = ExtractFeatures(obj);
		features.insert(features.end(), temp.begin(), temp.end());
		//tags.push_back(obj.tag);
	}
	Mat features_data(features.size()/FEATURES_COUNT, FEATURES_COUNT, CV_32FC1, &features[0]);
	//Mat tags_data(tags.size(), 1, CV_32FC1, &tags[0]);
	Mat predicted_tags;
	svm->predict(features_data, predicted_tags);
	for(unsigned i = 0; i < objects.size(); i++)
	{
		objects[i].tag = predicted_tags.at<float>(i);
	}
}
float TestModel(Ptr<SVM> svm, std::vector<Obj2d>& objects)
{
	float res;

	return res;
}