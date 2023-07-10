#include "stdafx.h"
#include "ImageObject.h"
#include <map>

#include "excercise1.h"

double Moment(int p, int q, ImageObject imageObj)
{
	int size = imageObj.GetSize();		// Get number of pixels in object
	int x, y;
	double result = 0;
	
	for (int i = 0; i < size; i++)
	{
		int x = imageObj.GetPixel(i)[0];
		int y = imageObj.GetPixel(i)[1];
		result += pow(x, p) * pow(y, q);
	}

	return result;
}

double NewMoment(int p, int q, cv::Mat& indexed, int id)
{
	double result = 0;
	for (int r = 0; r < indexed.rows; r++)
	{
		for (int c = 0; c < indexed.cols; c++)
		{
			if (indexed.at<char>(r, c) == id)
			{
				result += pow(c, p) * pow(r, q);
			}
		}
	}
	return result;
}

double CenterMoment(int p, int q, cv::Mat& indexed, int id)
{
	double x_center = NewMoment(1, 0, indexed, id) / NewMoment(0, 0, indexed, id);
	double y_center = NewMoment(0, 1, indexed, id) / NewMoment(0, 0, indexed, id);

	double result = 0;
	for (int r = 0; r < indexed.rows; r++)
	{
		for (int c = 0; c < indexed.cols; c++)
		{
			if (indexed.at<char>(r, c) == id)
			{
				result += pow(c - x_center, p) * pow(r - y_center, q);
			}
			
		}
	}

	return result;
}

double OldCenterMoment(int p, int q, ImageObject imageObj)
{
	int size = imageObj.GetSize();
	double center_x = Moment(1, 0, imageObj) / Moment(0, 0, imageObj);	// X center of mass
	double center_y = Moment(0, 1, imageObj) / Moment(0, 0, imageObj);	// Y center of mass

	double result = 0;
	for (int i = 0; i < size; i++)
	{
		int x = imageObj.GetPixel(i)[0];
		int y = imageObj.GetPixel(i)[1];

		result += pow((x - center_x), p) * pow((y - center_y), q);
	}

	return result;
}

int OldCircumference(cv::Mat &indexed, ImageObject imageObj)
{
	int id = imageObj.GetId();
	int size = imageObj.GetSize();
	int result = 0;

	for (int i = 0; i < size; i++)
	{
		int x = imageObj.GetPixel(i)[0];
		int y = imageObj.GetPixel(i)[1];

		int up, down, left, right;

		if (x == 0) { left = id; }
		else left = indexed.at<char>(y, x-1);

		if (x == indexed.cols - 1) { right = id; }
		else right = indexed.at<char>(y, x+1);

		if (y == 0) { up = id; }
		else up = indexed.at<char>(y-1, x);

		if (y == indexed.rows - 1) { down = id; }
		else down = indexed.at<char>(y+1, x);

		if (up != id || down != id || left != id || right != id)
		{
			result++;
		}
	}

	return result;
}

int Circumference(cv::Mat& indexed, int id)
{
	int result = 0;

	for (int r = 0; r < indexed.rows; r++)
	{
		for (int c = 0; c < indexed.cols; c++)
		{
			if (indexed.at<char>(r, c) == id)
			{
				int up, down, left, right;

				if (c == 0) { left = id; }
				else left = indexed.at<char>(r, c - 1);

				if (c == indexed.cols - 1) { right = id; }
				else right = indexed.at<char>(r, c + 1);

				if (r == 0) { up = id; }
				else up = indexed.at<char>(r - 1, c);

				if (r == indexed.rows - 1) { down = id; }
				else down = indexed.at<char>(r + 1, c);

				if (up != id || down != id || left != id || right != id)
				{
					result++;
				}
			}
		}
	}

	return result;
}

double Feature1(double area, double circumference)
{
	double result = pow(circumference, 2) / (100 * area);
	return result;
}

double Feature2(cv::Mat& indexed, int id)
{
	double mi_max, mi_min;

	double mi_20 = CenterMoment(2, 0, indexed, id);
	double mi_02 = CenterMoment(0, 2, indexed, id);
	double mi_11 = CenterMoment(1, 1, indexed, id);

	double p1 = 0.5 * (mi_20 + mi_02);
	double p2 = 0.5 * sqrt(4 * pow(mi_11, 2) + pow(mi_20 - mi_02, 2));

	mi_max = p1 + p2;
	mi_min = p1 - p2;

	return mi_min / mi_max;
}

// F1
/*
double OldFeature1(ImageObject &imageObj, double circumference)
{
	double area = CenterMoment(0, 0, imageObj);

	return pow(circumference, 2) / (100 * area);
}
*/

// F2
/*
double OldFeature2(ImageObject &imageObj)
{
	double max, min;

	max = ((double)1 / (double)2) * (CenterMoment(2, 0, imageObj) + CenterMoment(0, 2, imageObj)) +
		(((double)1 / (double)2) * sqrt(4 * pow(CenterMoment(1, 1, imageObj), 2) + pow(CenterMoment(2, 0, imageObj) + CenterMoment(0, 2, imageObj), 2)));

	min = ((double)1 / (double)2) * (CenterMoment(2, 0, imageObj) + CenterMoment(0, 2, imageObj)) -
		(((double)1 / (double)2) * sqrt(4 * pow(CenterMoment(1, 1, imageObj), 2) + pow(CenterMoment(2, 0, imageObj) + CenterMoment(0, 2, imageObj), 2)));

	return (min / max);
}
*/

void ex2()
{
	cv::Mat original = cv::imread("images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow("Original image", original);

	cv::Mat segmentation(original.rows, original.cols, CV_8SC1, cv::Scalar(0));	// Segmentation result

	int treshold = 1;

	Segmentation(treshold, original, segmentation);	// Segmentation

	int areaId = 1;
	for (int r = 0; r < segmentation.rows; r++)
	{
		for (int c = 0; c < segmentation.cols; c++)
		{
			if (segmentation.at<char>(r, c) == 0)
			{
				FloodFill(segmentation, r, c, areaId);	// Indexing using Flood Fill alg
				areaId++;
			}
		}
	}
	
	int objNum = 0;
	for (int i = 0; i < areaId; i++)
	{
		double area = CenterMoment(0, 0, segmentation, i);
		if (area > 10)
		{
			objNum++;
			int circumference = Circumference(segmentation, i);
			double F1 = Feature1(area, (double)circumference);
			double F2 = Feature2(segmentation, i);
			printf("Object %d- \t area: %f; \t circumference: %d; \t F1: %f; \t F2: %f \n", objNum, area, circumference, F1, F2);
		}
		
	}

	cv::Mat result(original.rows, original.cols, CV_8UC3, cv::Scalar(0, 0, 0));	// Result image
	ColorFill(segmentation, result);		// Coloring
	//cv::imshow("Found objects", result);	// Show result

	/*
	// Fill dictionary with found objects
	std::map<int, ImageObject> map_objects;		// Dictionary of ImageObjects
	for (int r = 0; r < segmentation.rows; r++)
	{
		for (int c = 0; c < segmentation.cols; c++)
		{
			int valueInPixel = segmentation.at<char>(r, c);
			if (valueInPixel > 0)
			{
				if (map_objects.count(valueInPixel))
				{
					map_objects[valueInPixel].AddPixel(r, c);	// Add pixel to existing object
				}
				else
				{
					map_objects.insert({ valueInPixel, ImageObject(valueInPixel) });
					map_objects[valueInPixel].AddPixel(r, c);
				}
			}
		}
	}

	int objNum = 0;
	for (int i = 0; i < map_objects.size(); i++)
	{
		ImageObject currentObject = map_objects[i];

		// Work only with big enough objects
		if (currentObject.GetSize() > 10)
		{
			objNum++;
			double area = Moment(0, 0, currentObject);
			int circumference = Circumference(segmentation, currentObject);
			double f1 = Feature1(currentObject, circumference);
			double f2 = Feature2(currentObject);

			printf("Object %d - \t Area: %1.f; \t Circumference: %d \t F1: %f \t F2: %f \n", objNum, area, circumference, f1, f2);
		}
	}
	*/
	
}