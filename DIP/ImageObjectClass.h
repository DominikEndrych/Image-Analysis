#pragma once
#include "stdafx.h"
#include <vector>
class ImageObjectClass
{
public:
	ImageObjectClass(int id, cv::Vec3b color);
	double GetEthalon();
	void AddFeature1(double value);
	void AddFeature2(double value);
private:
	int _id;
	cv::Vec3b _color;
	std::vector<double> _features1;
	std::vector<double> _features2;
};

