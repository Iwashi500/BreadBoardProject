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
	Rect position; //ç∂è„ç¿ïW
	vector<Point> holes;
	String type;

	Part(Mat mat, Rect position, int size, String type) {
		this->mat = mat;
		this->position = position;
		this->size = size;
		this->type = type;
	}

	void addHole(Point hole) {
		holes.push_back(hole);
	}

	Part(){}
};

