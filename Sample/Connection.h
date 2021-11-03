#pragma once
#include <opencv2/opencv.hpp>
#include "PartType.h"

using namespace std;
using namespace cv;

class Connection
{
public:
	Point point1;
	Point point2;
	PartType type;

	Connection(Point p1, Point p2, PartType type) {
		this->point1 = p1;
		this->point2 = p2;
		this->type = type;
	}

	bool operator==(const Connection chain) {
		return this->point1 == chain.point1 && this->point2 == chain.point2;
	}
	inline bool operator!=(const Connection chain) {
		return !(*this == chain);
	}
};

