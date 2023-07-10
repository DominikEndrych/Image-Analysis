#include "stdafx.h"
#include <vector>
#include "excercise1.h"
#include "excercise2.h"
#include "excercise3.h"
#include "excercise4.h"

double EuclidDistance(double x1, double x2, double y1, double y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void ComputeCentroids(cv::Mat &segmentation,std::vector<std::vector<double>> &centroids, int nObjects)
{
	// Not the best way but readable
	// All of these vectors should have the same dimension as number of centroids
	std::vector<double> xValues = { 0.f, 0.f, 0.f };
	std::vector<double> yValues = { 0.f, 0.f, 0.f };
	std::vector<int> valuesCout = { 0, 0, 0 };

	for (int i = 0; i < nObjects; i++)
	{
		double area = CenterMoment(0, 0, segmentation, i);
		if (area > 10)
		{
			int circumference = Circumference(segmentation, i);
			double F1 = Feature1(area, (double)circumference);
			double F2 = Feature2(segmentation, i);

			double minDistance = 100.f;
			int minDistanceIndex = 0;

			for (int c = 0; c < 3; c++)
			{
				std::vector<double> centroid = centroids[c];
				double distance = sqrt(pow(F1 - centroid[0], 2) + pow(F2 - centroid[1], 2));

				if (distance < minDistance)
				{
					minDistance = distance;
					minDistanceIndex = c;
				}
			}

			xValues[minDistanceIndex] += F1;
			yValues[minDistanceIndex] += F2;
			valuesCout[minDistanceIndex] += 1;
		}
	}

	// Reposition centroids
	for (int i = 0; i < 3; i++)
	{
		double newX = xValues[i] / valuesCout[i];
		double newY = yValues[i] / valuesCout[i];
		centroids[i] = { newX, newY };
	}
}

int GetClosesCentroid(std::vector<std::vector<double>> &centroids, double feature1, double feature2)
{
	double minDistance = 100.f;
	double distance = 0;
	int closesCentroid = 0;
	for (int c = 0; c < 3; c++)
	{
		std::vector<double> centroid = centroids[c];
		distance = EuclidDistance(feature1, centroid[0], feature2, centroid[1]);

		if (distance < minDistance)
		{
			minDistance = distance;
			closesCentroid = c;
		}
	}

	return closesCentroid;
}

void ex4()
{
	std::string trainingFile = "images/train.png";
	std::string testingFile = "images/test01.png";

	// Initialize centroids
	std::vector<double> centroid_1 = { 0.1f, 1.f };
	std::vector<double> centroid_2 = { 0.4f, 0.1f };
	std::vector<double> centroid_3 = { 0.9f, 0.8f };
	std::vector<std::vector<double>> centroids = { centroid_1, centroid_2, centroid_3 };

	cv::Mat original = cv::imread("images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	//cv::imshow("Original image", original);

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

	int cycles = 10;
	for (int i = 0; i < cycles; i++)
	{
		ComputeCentroids(segmentation, centroids, areaId);
	}
	

	// Color objects based on closest centroid
	cv::Vec3b colors[] = { cv::Vec3b(255,0,0), cv::Vec3b(0,255,0), cv::Vec3b(0,0,255) };

	cv::Mat testImage;
	int nObjects;		// For storing number of objects

	PrepareSegmentation(testImage, testingFile);
	PrepareIndexing(testImage, nObjects);

	cv::Mat result(testImage.rows, testImage.cols, CV_8UC3, cv::Scalar(0, 0, 0));	// Result image

	for (int i = 0; i < nObjects; i++)
	{
		double area = CenterMoment(0, 0, testImage, i);
		if (area > 10)
		{
			int circumference = Circumference(testImage, i);
			double f1 = Feature1(area, (double)circumference);
			double f2 = Feature2(testImage, i);


			int closestCentroid = GetClosesCentroid(centroids, f1, f2);		// Get closest centroid by F1 and F2

			// Coloring by closest centroid
			for (int r = 0; r < result.rows; r++)
			{
				for (int c = 0; c < result.cols; c++)
				{
					if (testImage.at<char>(r, c) == i)
					{
						result.at<cv::Vec3b>(r, c) = colors[closestCentroid];
					}
				}
			}
		}

	}

	cv::imshow("k-means", result);
	
	printf("\n");
	printf("Centroid 1 - { %f, %f }\n", centroids[0][0], centroids[0][1]);
	printf("Centroid 2 - { %f, %f }\n", centroids[1][0], centroids[1][1]);
	printf("Centroid 3 - { %f, %f }\n", centroids[2][0], centroids[2][1]);
}