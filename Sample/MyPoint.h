#pragma once
class MyPoint
{
public:
	int x;
	int y;
	MyPoint(int y, int x) {
		this->x = x;
		this->y = y;
	}
	MyPoint() {
		MyPoint(0, 0);
	}

	bool operator==(const MyPoint point) {
		return this->x == point.x && this->y == point.y;
	}
	inline bool operator!=(const MyPoint point) {
		return !(*this == point);
	}
};

