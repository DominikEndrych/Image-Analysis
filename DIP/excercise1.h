#pragma once
#include "stdafx.h"

void ex1();
void Segmentation(int treshold, cv::Mat &original, cv::Mat &result);
void FloodFill(cv::Mat &original, int r, int c, int id);
void ColorFill(cv::Mat& segmentation, cv::Mat& result);