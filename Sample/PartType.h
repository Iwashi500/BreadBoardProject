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
		SWITCH = 4,
		TRAN_NPN = 5,
		TRAN_PNP = 6,
		IC8 = 7,
		IC14 = 8,
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
		case Type::SWITCH:
			return "switch";
			break;
		case Type::TRAN_NPN:
			return "transistor_npn";
			break;
		case Type::TRAN_PNP:
			return "transistor_pnp";
			break;
		case Type::IC8:
			return "ic8";
			break;
		case Type::IC14:
			return "ic14";
			break;
		default:
			return "unknown";
			break;
		}
	}
};

