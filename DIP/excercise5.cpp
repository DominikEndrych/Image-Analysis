#pragma once
#include "stdafx.h"
#include <vector>
#include "bpnn/backprop.h"
#include "excercise3.h"
#include "excercise2.h"

void Train(NN* nn, double** inputVector, double** resultVector, int rows)
{
	double error = 1.0f;
	int i = 0;
	while (error > 0.005)
	{
		setInput(nn, inputVector[i%rows]);
		feedforward(nn);
		error = backpropagation(nn, resultVector[i%rows]);
		i++;
		printf("Error: %f \n", error);
	}

	// testing
	/*
	for (int i = 0; i < rows; i++)
	{
		setInput(nn, inputVector[i], true);
		feedforward(nn);
		int output = getOutput(nn, true);
	}
	*/
}

void Test(NN* nn, std::string filename)
{
	cv::Mat testImage;
	int nObjects;

	cv::Vec3b colors[] = { cv::Vec3b(255,0,0), cv::Vec3b(0,255,0), cv::Vec3b(0,0,255) };

	PrepareSegmentation(testImage, filename);
	PrepareIndexing(testImage, nObjects);

	cv::Mat result(testImage.rows, testImage.cols, CV_8UC3, cv::Scalar(0, 0, 0));	// Result image

	int objectNumber = 0;

	for (int i = 0; i < nObjects; i++)
	{
		double area = CenterMoment(0, 0, testImage, i);
		if (area > 10)
		{
			int circumference = Circumference(testImage, i);
			double f1 = Feature1(area, (double)circumference);
			double f2 = Feature2(testImage, i);

			double* inputs = new double[2];
			inputs[0] = f1;
			inputs[1] = f2;

			setInput(nn, inputs, true);
			feedforward(nn);
			int output = getOutput(nn, true);

			for (int r = 0; r < result.rows; r++)
			{
				for (int c = 0; c < result.cols; c++)
				{
					if (testImage.at<char>(r, c) == i)
					{
						result.at<cv::Vec3b>(r, c) = colors[output];
					}
				}
			}

			//objectNumber++;
		}
	}

	cv::imshow("Backpropagation test", result);
}

void ex5()
{
	std::string trainingFile = "images/train.png";
	std::string testingFile = "images/test01.png";

	cv::Mat original = cv::imread("images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	//cv::imshow("Original image", original);

	cv::Mat trainImage;
	int nObjects;		// For storing number of objects

	PrepareSegmentation(trainImage, trainingFile);
	PrepareIndexing(trainImage, nObjects);

	int n = 12;

	double ** inputVector = new double*[n];
	double** resultVector = new double* [n];
	for (int i = 0; i < n; i++)
	{
		inputVector[i] = new double[2];
		resultVector[i] = new double[3];
	}

	int objectNumber = 0;
	std::vector<int> classes = { 1,1,1,1,2,2,2,2,3,3,3,3 };	// Objects in training image

	for (int i = 0; i < nObjects; i++)
	{
		double area = CenterMoment(0, 0, trainImage, i);
		if (area > 10)
		{
			int circumference = Circumference(trainImage, i);
			double f1 = Feature1(area, (double)circumference);
			double f2 = Feature2(trainImage, i);

			inputVector[objectNumber][0] = f1;
			inputVector[objectNumber][1] = f2;
			
			int objectClass = classes[objectNumber];
			if (objectClass == 1)
			{
				resultVector[objectNumber][0] = 1.0;
				resultVector[objectNumber][1] = 0.0;
				resultVector[objectNumber][2] = 0.0;
			}
			if (objectClass == 2)
			{
				resultVector[objectNumber][0] = 0.0;
				resultVector[objectNumber][1] = 1.0;
				resultVector[objectNumber][2] = 0.0;
			}
			if (objectClass == 3)
			{
				resultVector[objectNumber][0] = 0.0;
				resultVector[objectNumber][1] = 0.0;
				resultVector[objectNumber][2] = 1.0;
			}
			objectNumber++;
		}
	}

	NN* nn = createNN(2, 4, 3);
	Train(nn, inputVector, resultVector, n);
	Test(nn, testingFile);

	printf("\nZelena - Hvezda\n");
	printf("Modra - Ctverec\n");
	printf("Cervena - Obdelnik\n");
}