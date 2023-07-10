#pragma once

void ex3();
void PrepareSegmentation(cv::Mat& result, std::string file);
void PrepareIndexing(cv::Mat& segmentation, int& objects);
