#pragma once
#include <list>
#include "MyPoint.h"
#include "ChainPoint.h"
#include "OnLine.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
class BreadBoard
{
public:
	vector<Point> usedHoles;
	vector<Point> unusedHoles;
	vector<vector<Point>> holePositions;

	static String getHoleName(Point position);


	/*void push(OnLine onLine) {
		onLines.push_back(onLine);
	}

	std::list<OnLine> getOnLines() {
		return this->onLines;
	}

	OnLine getOnLine(MyPoint point);*/
};



