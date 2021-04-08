#include "pch.h"
#include "BreadBoard.h"

OnLine BreadBoard::getOnLine(MyPoint point) {
	for (OnLine onLine : onLines) {
		std::list<MyPoint> points = onLine.getPoints();
		auto result = std::find(points.begin(), points.end(), point);
		if (result != points.end()) {
			return onLine;
		}
	}

	return OnLine();
}
