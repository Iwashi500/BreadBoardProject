#include "pch.h"
#include "BreadBoard.h"

String BreadBoard::getHoleName(Point position) {
	String xName = format("%d", position.x + 1);
	String yName;
	switch (position.y)
	{
	case 0:
		yName = "1-";
		break;
	case 1:
		yName = "1+";
		break;
	case 2:
		yName = "j";
		break;
	case 3:
		yName = "i";
		break;
	case 4:
		yName = "h";
		break;
	case 5:
		yName = "g";
		break;
	case 6:
		yName = "f";
		break;
	case 7:
		yName = "e";
		break;
	case 8:
		yName = "d";
		break;
	case 9:
		yName = "c";
		break;
	case 10:
		yName = "b";
		break;
	case 11:
		yName = "a";
		break;
	case 12:
		yName = "2-";
		break;
	case 13:
		yName = "2+";
		break;
	default:
		yName = "unknow";
		break;
	}

	return xName + "-" + yName;
}

bool BreadBoard::checkUsed(int y, int x) {
	return this->checkUsed(Point(x, y));
}

bool BreadBoard::checkUsed(Point position) {
	if (position.y < 0 || position.x < 0 || position.y > 13 || position.x > 29)
		return false;
	else if ((position.y <= 1 || 12 <= position.y) && (position.x > 25))
		return false;

	return find(usedHoles.begin(), usedHoles.end(), position) != usedHoles.end();
}

vector<Point>::iterator BreadBoard::deleteUsedHole(Point hole) {
	auto ite = find(usedHoles.begin(), usedHoles.end(), hole);

	if (ite != usedHoles.end()) {
		return usedHoles.erase(ite);
	}
	else
		return ite;
}

//OnLine BreadBoard::getOnLine(MyPoint point) {
//	for (OnLine onLine : onLines) {
//		std::list<MyPoint> points = onLine.getPoints();
//		auto result = std::find(points.begin(), points.end(), point);
//		if (result != points.end()) {
//			return onLine;
//		}
//	}
//
//	return OnLine();
//}
