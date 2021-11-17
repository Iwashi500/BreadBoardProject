#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include "PartType.h"
using namespace cv;
using namespace std;

class Part
{
public:
	int size;
	Mat mat;
	Rect position; //ƒp[ƒc‰æ‘œ‚Ì—Ìˆæ
	vector<Point> holes;
	PartType partType;
	
	Part(Mat mat, Rect position, int size, PartType type) {
		this->mat = mat;
		this->position = position;
		this->size = size;
		this->partType = type;
	}

	void addHole(Point hole) {
		holes.push_back(hole);
		size++;
	}

	Part(){}
	virtual PartType getPartType() { return partType; }
};

