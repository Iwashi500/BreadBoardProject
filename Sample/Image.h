#pragma once

#include "MyPoint.h"

class Image
{
public:
	int Height; //高さ
	int Width; //幅
	unsigned char** R, ** G, ** B; //RGBデータ格納用

	//引数付きコンストラクタ
	Image(int h, int w);
	//デストラクタ（仮想関数）
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

