#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include "PartType.h"
#include "Part.h"
using namespace cv;
using namespace std;


#include "Part.h"
class PartLED : public Part
{
public:
    Mat head;

    PartLED(Mat mat, Rect position, int size, Mat head) {
        this->mat = mat;
        this->position = position;
        this->size = size;
        this->partType = PartType::LED;
        this->head = head;
    }

    PartType getPartType() override { return PartType::LED; }
};

