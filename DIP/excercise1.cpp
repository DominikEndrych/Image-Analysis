#include "stdafx.h"

void Segmentation(int treshold, cv::Mat &original, cv::Mat &result)
{
	for (int r = 0; r < original.rows; r++)
	{
		for (int c = 0; c < original.cols; c++)
		{
			if (original.at<uchar>(r, c) > treshold)
			{
				result.at<char>(r, c) = 0;
			}
			else
			{
				result.at<char>(r, c) = -1;		// Define wall as -1 in segmentation
			}
		}
	}
}

void FloodFill(cv::Mat &original, int r, int c, int id)
{
	if (r < 0 || c < 0 || r >= original.rows || c >= original.cols) { return; }			// Index out of image
	if (original.at<char>(r, c) == id || original.at<char>(r, c) == -1) { return; }		// Already marked or wall

	original.at<char>(r, c) = id;	// Mark this position

	FloodFill(original, r + 1, c, id);
	FloodFill(original, r - 1, c, id);
	FloodFill(original, r, c + 1, id);
	FloodFill(original, r, c - 1, id);
}

void ColorFill(cv::Mat &segmentation, cv::Mat &result)
{
	// Colors to separate found objects
	cv::Vec3b colors[] = {
		cv::Vec3b(255,0,0), cv::Vec3b(0,255,0), cv::Vec3b(0,255,0),
		cv::Vec3b(255,255,0), cv::Vec3b(255,0,255), cv::Vec3b(0,255,255),
		cv::Vec3b(255,128,0), cv::Vec3b(255,0,128), cv::Vec3b(0,255,128),
		cv::Vec3b(128,255,0), cv::Vec3b(128,0,255), cv::Vec3b(0,128,255),
		cv::Vec3b(128,255,255), cv::Vec3b(128,255,255), cv::Vec3b(255,128,255),
		cv::Vec3b(64,255,255), cv::Vec3b(128,64,255), cv::Vec3b(255,128,64)
	};

	// Draw colors based on ID
	for (int r = 0; r < segmentation.rows; r++)
	{
		for (int c = 0; c < segmentation.cols; c++)
		{
			if (segmentation.at<char>(r, c) != -1)
			{
				result.at<cv::Vec3b>(r, c) = colors[segmentation.at<char>(r, c)];
			}
		}
	}
}

void ex1()
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

	cv::Mat result(original.rows, original.cols, CV_8UC3, cv::Scalar(0, 0, 0));	// Result image

	ColorFill(segmentation, result);	// Coloring

	cv::imshow("Indexing result", result);	// Show result

	

}