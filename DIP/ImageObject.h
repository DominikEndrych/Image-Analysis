#pragma once
#include <vector>

/*
	This is a helper class for image clasification.
	Some common attributes are stored here.
*/

class ImageObject
{
public:
	ImageObject();
	ImageObject(int);
	~ImageObject();
	void AddPixel(int, int);
	std::vector<int> GetPixel(int);
	int GetSize();
	int GetId();
private:
	int id;
	std::vector<std::vector<int>> pixels;	// {x,y} values of each pixel
};

