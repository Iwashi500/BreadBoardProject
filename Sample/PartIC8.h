#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include "PartType.h"
#include "Part.h"
using namespace cv;
using namespace std;

class PartIC8 : public Part
{
public:

    PartIC8(Mat mat, Rect position, int size) {
        this->mat = mat;
        this->position = position;
        this->size = size;
        this->partType = PartType::IC8;
    }

    PartType getPartType() override { return PartType::IC8; }
};

