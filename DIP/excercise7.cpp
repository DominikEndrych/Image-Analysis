#pragma once
#include "stdafx.h"
#include <vector>

struct Cluster
{
	int R;
	int G;
	int B;

	int x;
	int y;

	std::vector<Cluster> pixels;	// Pixels can be represented in the same way as Cluster
};

void ClearClusters(std::vector<Cluster>& clusters)
{
	for (int i = 0; i < clusters.size(); i++)
	{
		clusters[i].pixels.clear();
	}
}

void DrawClusters(std::vector<Cluster> clusters, cv::Mat& image, std::string windowName)
{
	cv::Mat clusters_image;
	image.copyTo(clusters_image);

	int nClusters = clusters.size();
	for (int i = 0; i < clusters.size(); i++)
	{
		cv::circle(clusters_image, cv::Point(clusters[i].x, clusters[i].y), 4, (0, 0, 255), -1);
	}

	cv::imshow(windowName, clusters_image);
}

float GetClusterDistance(int r, int g, int b, int x, int y, Cluster &cluster, float m, int S)
{
	float d_rgb = sqrt(pow(cluster.R - r, 2) + pow(cluster.G - g, 2) + pow(cluster.B - b, 2));
	float d_xy = sqrt(pow(cluster.x - x, 2) + pow(cluster.y - y, 2));
	float D_s = d_rgb + (m / (float)S) * d_xy;
	return D_s;
}

void RepositionClusters(std::vector<Cluster>& clusters)
{
	for (int i = 0; i < clusters.size(); i++)
	{
		if (clusters[i].pixels.size() > 0)
		{
			float sum_r = 0, sum_g = 0, sum_b = 0, sum_x = 0, sum_y = 0;
			for (int j = 0; j < clusters[i].pixels.size(); j++)
			{
				sum_r += (float)clusters[i].pixels[j].R;
				sum_g += (float)clusters[i].pixels[j].G;
				sum_b += (float)clusters[i].pixels[j].B;
				sum_x += (float)clusters[i].pixels[j].x;
				sum_y += (float)clusters[i].pixels[j].y;
			}
			clusters[i].R = sum_r / clusters[i].pixels.size();
			clusters[i].G = sum_g / clusters[i].pixels.size();
			clusters[i].B = sum_b / clusters[i].pixels.size();
			clusters[i].x = sum_x / clusters[i].pixels.size();
			clusters[i].y = sum_y / clusters[i].pixels.size();
			//printf("\nCluster adjusted");
		}
		//printf("Repositioning done");
	}

	/*
	float mean_r, mean_g, mean_b, mean_x, mean_y;
	for (int i = 0; i < clusters.size(); i++)
	{
		Cluster cluster = clusters[i];
		if (cluster.pixels.size() > 0)
		{
			float sum_r = 0, sum_g = 0, sum_b = 0, sum_x = 0, sum_y = 0;
			for (int j = 0; j < cluster.pixels.size(); j++)
			{

				sum_r += cluster.pixels[j].R;
				sum_g += cluster.pixels[j].G;
				sum_b += cluster.pixels[j].B;
				sum_x += cluster.pixels[j].x;
				sum_y += cluster.pixels[j].y;
			}
			//cluster.R = (float)sum_r / cluster.pixels.size();
			cluster.R = 0;
			cluster.G = (float)sum_g / cluster.pixels.size();
			cluster.B = (float)sum_b / cluster.pixels.size();
			cluster.x = (float)sum_x / cluster.pixels.size();
			cluster.y = (float)sum_y / cluster.pixels.size();

			clusters[i] = cluster;
			printf("\nCluster %d done", i);
		}
	}
	*/
}

void AssignPixels(std::vector<Cluster> &clusters, cv::Mat &original, int S, float m)
{
	std::vector<Cluster> result;
	result = clusters;

	for (int r = 0; r < original.rows; r++)
	{
		for (int c = 0; c < original.cols; c++)
		{
			cv::Vec3b color = original.at<cv::Vec3b>(r, c);
			int closeIndex = 0;
			float D_s = 99999.9f;

			// Count distance to each cluster
			// TODO: Count only close clusters
			for (int i = 0; i < result.size(); i++)
			{
				float distance = GetClusterDistance(color[2], color[1], color[0], c, r, result[i], m, S);
				if (distance < D_s)
				{
					closeIndex = i;
					D_s = distance;
				}
			}
			result[closeIndex].pixels.push_back({ color[2], color[1], color[0], c, r });
		}
		//printf("\n %d row", r);
	}
	clusters = result;
	/*
	std::vector<Cluster> result;
	result = clusters;
	for (int r = 0; r < original.rows; r++)
	{
		for (int c = 0; c < original.cols; c++)
		{
			int x_min, x_max, y_min, y_max;
			x_min = c - (2 * S) < 0 ? 0 : c - (2 * S);
			x_max = c + (2 * S) < 0 ? 0 : c + (2 * S);
			y_min = r - (2 * S) < 0 ? 0 : r - (2 * S);
			y_max = r + (2 * S) < 0 ? 0 : r + (2 * S);

			int color_r= original.at<cv::Vec3b>(r, c)[2];
			int color_g = original.at<cv::Vec3b>(r, c)[1];
			int color_b = original.at<cv::Vec3b>(r, c)[0];

			float D_s = 9999.0f;
			Cluster closestCluster = clusters[0];
			int closestIndex = 0;

			for (int i = 0; i < clusters.size(); i++)
			{
				Cluster current = clusters[i];
				// Compare only clusters in 2S x 2S area
				if (1)
				{
					float distance = GetClusterDistance(color_r, color_g, color_b, c, r, current, S, m);;
					if (distance < D_s)
					{
						//closestCluster = current;
						closestIndex = i;
					}
				}
			}
			result[closestIndex].pixels.push_back({ color_r,color_g,color_b,c,r });		// Assign pixel to a closest cluster
		}
		//printf("\n %d row", r);
	}
	clusters = result;
	*/
}

float L2Norm(std::vector<int> a)
{
	int sum = 0;
	for (int i = 0; i < a.size(); i++)
	{
		sum += pow(a[i], 2);
	}
	return sqrt(sum);
}

float ComputeGradient(cv::Mat &original, int row, int column)
{
	cv::Vec3b first = original.at<cv::Vec3b>(row, column + 1) - original.at<cv::Vec3b>(row, column - 1);
	cv::Vec3b second = original.at<cv::Vec3b>(row + 1, column) - original.at<cv::Vec3b>(row - 1, column);
	return L2Norm({ first[0], first[1], first[2] }) + L2Norm({ second[0], second[1], second[2] });
}

void SLIC(int K, cv::Mat &original)
{
	cv::Mat clusters_img;
	original.copyTo(clusters_img);

	std::vector<Cluster> clusters;

	float N = original.cols * original.rows;
	float S = sqrt(N / K);

	// Initialize cluster grid
	for (int r = S/2; r < original.rows; r += S)
	{
		for (int c = S/2; c < original.cols; c += S)
		{

			// Compute lowest gradient in 3x3
			int low_r = r, low_c = c;
			float low_grad = ComputeGradient(original, r, c);
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					float grad = ComputeGradient(original, r + i, c + j);
					if (grad < low_grad)
					{
						low_r = r + i;
						low_c = c + j;
						low_grad = grad;
					}
				}
			}
			
			// Initialize cluster
			cv::Vec3b clusterValue = original.at<cv::Vec3b>(low_r, low_c);
			Cluster toAdd = Cluster();
			toAdd.x = low_c;
			toAdd.y = low_r;
			toAdd.R = clusterValue[2];
			toAdd.G = clusterValue[1];
			toAdd.B = clusterValue[0];
			clusters.push_back(toAdd);
		}
	}

	DrawClusters(clusters, original, "Initial clusters");	// Draw initial clusters

	float m = 15.0f;	// Position weight parameter
	int cycles = 10;

	// Move clusters
	for (int i = 0; i < cycles; i++)
	{
		ClearClusters(clusters);
		//printf("\nAssigning pixels...");
		AssignPixels(clusters, original, S, m);
		//printf("\nRepositioning clusters...");
		RepositionClusters(clusters);
	}
	DrawClusters(clusters, original, "Repositioned");


	// Drawing result
	for (int i = 0; i < clusters.size(); i++)
	{
		cv::Vec3b finalColor;
		finalColor[0] = clusters[i].B;
		finalColor[1] = clusters[i].G;
		finalColor[2] = clusters[i].R;

		for (int j = 0; j < clusters[i].pixels.size(); j++)
		{
			original.at<cv::Vec3b>(clusters[i].pixels[j].y, clusters[i].pixels[j].x) = finalColor;
		}
	}

	cv::imshow("Final picture", original);
	
}

void ex7()
{
	std::string filename = "images/lena.png";

	cv::Mat original = cv::imread(filename, cv::IMREAD_COLOR);

	//cv::imshow("Original image", original);
	SLIC(9, original);
}