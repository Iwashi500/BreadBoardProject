#pragma once
#include "Connection.h"

class ConnectionTransistor :
    public Connection
{
public:
	Point point3;

	ConnectionTransistor(Point p1, Point p2, Point p3, PartType type) {
		this->point1 = p1;
		this->point2 = p2;
		this->point3 = p3;
		this->type = type;
	}

	bool operator==(const ConnectionTransistor chain) {
		return this->point1 == chain.point1 && this->point2 == chain.point2 && this->point3 == chain.point3;
	}
	inline bool operator!=(const ConnectionTransistor chain) {
		return !(*this == chain);
	}

	PartType getPartType() override { return type; }
};

