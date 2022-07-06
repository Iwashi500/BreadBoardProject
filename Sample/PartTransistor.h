#pragma once
#include "Part.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include "PartType.h"
using namespace cv;
using namespace std;


#include "Part.h"
class PartTransistor :
    public Part
{
public:
    double degree;

    PartTransistor(Mat mat, Rect position, int size, double degree, PartType type) {
        this->mat = mat;
        this->position = position;
        this->size = size;
        this->partType = type;
        this->degree = degree;
    }

    PartType getPartType() override { return partType; }
};