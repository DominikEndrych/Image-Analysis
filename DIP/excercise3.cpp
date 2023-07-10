#include "stdafx.h"
#include <string>
#include <vector>
#include <map>
#include "ImageObjectClass.h"
#include "excercise1.h"
#include "excercise2.h"

void PrepareSegmentation(cv::Mat &result, std::string file)
{
	cv::Mat original = cv::imread(file, CV_LOAD_IMAGE_GRAYSCALE);
	//cv::imshow("Original image", original);

	cv::Mat segmentation(original.rows, original.cols, CV_8SC1, cv::Scalar(0));	// Segmentation result

	int treshold = 1;

	Segmentation(treshold, original, segmentation);	// Segmentation

	segmentation.copyTo(result);
}

void PrepareIndexing(cv::Mat& segmentation, int& objects)
{
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
	objects = areaId;
}
void Train(std::string filename, std::vector<int> objectClasses, int objectsInClass, std::vector<double> &vector_f1, std::vector<double> &vector_f2 )
{
	cv::Mat segmentation_t;

	PrepareSegmentation(segmentation_t, filename);
	int objects;
	PrepareIndexing(segmentation_t, objects);

	/*
	cv::Mat result(segmentation_t.rows, segmentation_t.cols, CV_8UC3, cv::Scalar(0, 0, 0));	// Result image
	ColorFill(segmentation_t, result);		// Coloring
	cv::imshow("Found objects", result);	// Show result
	*/

	std::vector<double> resultFeatures;

	int objectNum = 0;
	for (int i = 1; i < objects; i++)
	{
		double area = CenterMoment(0, 0, segmentation_t, i);
		if (area > 10)
		{
			int circumference = Circumference(segmentation_t, i);
			double feature1 = Feature1(area, (double)circumference);
			double feature2 = Feature2(segmentation_t, i);

			int currentClass = objectClasses[objectNum];
			vector_f1[currentClass - 1] += feature1;
			vector_f2[currentClass - 1] += feature2;
			objectNum++;
		}
	}
}

int CompareFeatures(double f1, double f2, std::vector<double> vector_f1, std::vector<double> vector_f2, int objects)
{
	double minError = 10000;
	int c = 1;

	for (int i = 0; i < objects; i++)
	{	
		double e1 = vector_f1[i] / 4;
		double e2 = vector_f2[i] / 4;

		double distance = sqrt(pow(f1 - e1, 2) + pow(f2 - e2, 2));
		if (distance < minError)
		{
			minError = distance;
			c = i;
		}
	}

	return c;
}

void Classification(std::string filename, std::vector<double> vector_f1, std::vector<double> vector_f2, int objectsNum)
{
	cv::Mat segmentation_t;

	PrepareSegmentation(segmentation_t, filename);
	int objects;
	PrepareIndexing(segmentation_t, objects);

	cv::Vec3b colors[] = { cv::Vec3b(255,0,0), cv::Vec3b(0,255,0), cv::Vec3b(0,0,255) };

	cv::Mat result(segmentation_t.rows, segmentation_t.cols, CV_8UC3, cv::Scalar(0, 0, 0));	// Result image

	for (int i = 0; i < objects; i++)
	{
		double area = CenterMoment(0, 0, segmentation_t, i);
		int circumference = Circumference(segmentation_t, i);
		double f1 = Feature1(area, (double)circumference);
		double f2 = Feature2(segmentation_t, i);

		int classIndex = CompareFeatures(f1, f2, vector_f1, vector_f2, objectsNum);

		for (int r = 0; r < result.rows; r++)
		{
			for (int c = 0; c < result.cols; c++)
			{
				if (segmentation_t.at<char>(r, c) == i)
				{
					result.at<cv::Vec3b>(r, c) = colors[classIndex];
				}
			}
		}

	}
	printf("1 - ctverec - modra \n2 - hvezda - zelena \n3 - obdelnik - cervena");
	cv::imshow("Ethalons", result);	// Show result
}

void ex3()
{
	std::string trainingFile = "images/train.png";
	std::string testingFile = "images/test01.png";

	// Objects in the image
	std::vector<int> classes = { 1,1,1,1,2,2,2,2,3,3,3,3 };

	std::vector<double> vector_f1 = { 0,0,0 };
	std::vector<double> vector_f2 = { 0,0,0 };

	Train(trainingFile, classes, 3, vector_f1, vector_f2);
	Classification(testingFile, vector_f1, vector_f2, 3);
}