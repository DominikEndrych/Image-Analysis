#include "stdafx.h"
#include "ImageObjectClass.h"

ImageObjectClass::ImageObjectClass(int id, cv::Vec3b color)
{
	_id = id;
	_color = color;
}

void ImageObjectClass::AddFeature1(double value)
{
	_features1.push_back(value);
}

void ImageObjectClass::AddFeature2(double value)
{
	_features2.push_back(value);
}

double ImageObjectClass::GetEthalon()
{
	double result = 0;
	
	for (int i = 0; i < _features1.size(); i++)
	{
		result += _features1[i];
		result += _features2[i];
	}
	
	return result = _features1.size();
}
