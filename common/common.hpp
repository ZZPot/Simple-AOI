#pragma once
#include <string>
#include <windows.h>
#include <opencv2/core.hpp>

#pragma warning(disable: 4244 4996)

#define RANDOM_NAME_MAX_CHARS 30
#define RANDOM_CHUNK_SIZE	5

std::string CreateRandomName(unsigned num_chars, std::string prefix = "", std::string postfix = "");

// I think OpenCV need this
#pragma region cv::Point_ OPERATORS
template<class t1, class t2> cv::Point_<t1> operator-(cv::Point_<t1> p1, cv::Point_<t2> p2)
{
	p1.x -= p2.x;
	p1.y -= p2.y;
	return p1;
}
template<class t1, class t2> cv::Point_<t1>& operator-=(cv::Point_<t1>& p1, cv::Point_<t2> p2)
{
	p1.x -= p2.x;
	p1.y -= p2.y;
	return p1;
}
template<class t1, class t2> cv::Point_<t1> operator+(cv::Point_<t1> p1, cv::Point_<t2> p2)
{
	p1.x += p2.x;
	p1.y += p2.y;
	return p1;
}
template<class t1, class t2> cv::Point_<t1>& operator+=(cv::Point_<t1>& p1, cv::Point_<t2> p2)
{
	p1.x += p2.x;
	p1.y += p2.y;
	return p1;
}
template<class t1, class t2> cv::Point_<t1> operator-(cv::Point_<t1> p1, t2 scalar)
{
	p1.x -= scalar;
	p1.y -= scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1>& operator-=(cv::Point_<t1>& p1, t2 scalar)
{
	p1.x -= scalar;
	p1.y -= scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1> operator+(cv::Point_<t1> p1, t2 scalar)
{
	p1.x += scalar;
	p1.y += scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1>& operator+=(cv::Point_<t1>& p1, t2 scalar)
{
	p1.x += scalar;
	p1.y += scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1> operator*(cv::Point_<t1> p1, t2 scalar)
{
	p1.x *= scalar;
	p1.y *= scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1>& operator*=(cv::Point_<t1>& p1, t2 scalar)
{
	p1.x *= scalar;
	p1.y *= scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1> operator/(cv::Point_<t1> p1, t2 scalar)
{
	p1.x /= scalar;
	p1.y /= scalar;
	return p1;
}
template<class t1, class t2> cv::Point_<t1>& operator/=(cv::Point_<t1>& p1, t2 scalar)
{
	p1.x /= scalar;
	p1.y /= scalar;
	return p1;
}
template<class t1> cv::Point_<t1> operator-(cv::Point_<t1> p1)
{
	p1.x *= -1;
	p1.y *= -1;
	return p1;
}
#pragma endregion