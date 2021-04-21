#pragma once

#include "MyPoint.h"

class Image
{
public:
	int Height; //����
	int Width; //��
	unsigned char** R, ** G, ** B; //RGB�f�[�^�i�[�p

	//�����t���R���X�g���N�^
	Image(int h, int w);
	//�f�X�g���N�^�i���z�֐��j
	virtual ~Image();
	bool checkColor(int i, int j, int r, int g, int b);
	bool checkBlack(int i, int j);
	bool checkWhite(int i, int j);
	Image* copy();

	void expansion(Image* image, int count);
	void contraction(int count);
	void drawLine(MyPoint point1, MyPoint point2);
	void cutRect(MyPoint upLeft, MyPoint downRight);
	void drawPoint(MyPoint point, int size);
	bool checkInnerImage(int y, int x);
};

