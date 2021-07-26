#pragma once
#include <list>
#include "MyPoint.h"
#include "Connection.h"
#include "OnLine.h"
#include "Part.h"
#include "HoleType.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
class BreadBoard
{
public:
	vector<Point> usedHoles;
	vector<Point> unusedHoles;
	vector<vector<HoleType>> holeTypes;
	vector<vector<Point>> holePositions;
	vector<Part> parts;
	vector<Connection> connections;

	static String getHoleName(Point position);

	bool checkUsed(int y, int x);
	bool checkUsed(Point position);
	vector<Point>::iterator deleteUsedHole(Point hole);

	Point getHolePosition(Point position) {
		return this->holePositions.at(position.y).at(position.x);
	}

	HoleType getHoleType(Point position) {
		return this->holeTypes.at(position.y).at(position.x);
	}

	/*void push(OnLine onLine) {
		onLines.push_back(onLine);
	}

	std::list<OnLine> getOnLines() {
		return this->onLines;
	}

	OnLine getOnLine(MyPoint point);*/
};



