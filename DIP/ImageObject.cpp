#include "stdafx.h"
#include "ImageObject.h"

ImageObject::ImageObject()
{
	return;
}

ImageObject::ImageObject(int id)
{
	this->id = id;
}

ImageObject::~ImageObject() { return; }

void ImageObject::AddPixel(int row, int column)
{
	//std::vector<int> toAdd = {column, row};

	this->pixels.push_back({column, row});
}

std::vector<int> ImageObject::GetPixel(int index)
{
	return this->pixels[index];
}

int ImageObject::GetSize()
{
	int size = this->pixels.size();
	return size;
}

int ImageObject::GetId()
{
	return this->id;
}