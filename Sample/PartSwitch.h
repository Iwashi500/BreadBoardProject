#pragma once
#include "Part.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include "PartType.h"
using namespace cv;
using namespace std;

class PartSwitch :
    public Part
{
public:
    double degree;

    PartSwitch(Mat mat, Rect position, int size, double degree) {
        this->mat = mat;
        this->position = position;
        this->size = size;
        this->partType = PartType::SWITCH;
        this->degree = degree;
    }

    PartType getPartType() override { return PartType::SWITCH; }
};

