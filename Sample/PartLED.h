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
    Point headPosition;

    PartLED(Mat mat, Rect position, int size, Mat head, Point headPosition) {
        this->mat = mat;
        this->position = position;
        this->size = size;
        this->partType = PartType::LED;
        this->head = head;
        this->headPosition = headPosition;
    }

    PartType getPartType() override { return PartType::LED; }
};

