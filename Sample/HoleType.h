#pragma once

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


class HoleType
{
public:
	enum Type
	{
		EMPTY = 0,
		MIDDLE = 1,
		EDGE = 2
	};

	HoleType() {}
	HoleType(Type type){
		this->type = type;
	}

	Type type;

	String toString() {
		switch (this->type)
		{
		case Type::EMPTY:
			return "empty";
			break;
		case Type::MIDDLE:
			return "middle";
			break;
		case Type::EDGE:
			return "edge";
			break;
		default:
			return "unknown";
			break;
		}
	}
};

