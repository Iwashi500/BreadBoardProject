#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Part
{
public:
	int size;
	Mat mat;
	Rect position; //¶ãÀ•W

	Part(Mat mat, Rect position, int size) {
		this->mat = mat;
		this->position = position;
		this->size = size;
	}

	Part(){}
};

