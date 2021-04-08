#pragma once
#include "MyPoint.h"

class ChainPoint
{
public:
	MyPoint point1;
	MyPoint point2;

	ChainPoint(MyPoint p1, MyPoint p2) {
		this->point1 = p1;
		this->point2 = p2;
	}

	void pushNext(MyPoint point) {
		this->point1 = this->point2;
		this->point2 = point;
	}

	bool operator==(const ChainPoint chain) {
		return this->point1 == chain.point1 && this->point2 == chain.point2;
	}
	inline bool operator!=(const ChainPoint chain) {
		return !(*this == chain);
	}
};

