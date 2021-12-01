#pragma once
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class PartType
{
public:
	enum Type
	{
		WIRE = 0,
		RESISTOR = 1,
		LED = 2,
		CONDENSER = 3,
	};

	PartType() {}
	PartType(Type type) {
		this->type = type;
	}

	Type type;

	String toString() {
		switch (this->type)
		{
		case Type::WIRE:
			return "wire";
			break;
		case Type::RESISTOR:
			return "resistor";
			break;
		case Type::LED:
			return "LED";
			break;
		case Type::CONDENSER:
			return "condenser";
			break;
		default:
			return "unknown";
			break;
		}
	}
};

