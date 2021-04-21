#include "pch.h"
#include "Image.h"
#include "MyPoint.h"
#include "iostream"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

//引数付きコンストラクタ
Image::Image(int h, int w)
{
	//高さ、幅
	Height = h;
	Width = w;

	//メモリエラー対策
	R = NULL;
	G = NULL;
	B = NULL;

	//メモリ確保
	R = new unsigned char* [Height];
	G = new unsigned char* [Height];
	B = new unsigned char* [Height];
	for (int i = 0; i < Height; i++) {
		R[i] = new unsigned char[Width];
		G[i] = new unsigned char[Width];
		B[i] = new unsigned char[Width];
	}
}

//デストラクタ
Image::~Image() {
	//メモリ開放
	if (R) {
		for (int i = 0; i < Height; i++) {
			delete[] R[i];
			delete[] G[i];
			delete[] B[i];
		}
		delete[] R;
		delete[] G;
		delete[] B;
	}
}

bool Image::checkColor(int i, int j, int r, int g, int b) {
	return (R[i][j] == r
		&& G[i][j] == g
		&& B[i][j] == b);
}

bool Image::checkBlack(int i, int j) {
	return checkColor(i, j, 0, 0, 0);
}

bool Image::checkWhite(int i, int j) {
	return checkColor(i, j, 255, 255, 255);
}

Image* Image::copy() {
	Image* image = new Image(Height, Width);

	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			image->R[i][j] = R[i][j];
			image->G[i][j] = G[i][j];
			image->B[i][j] = B[i][j];
		}
	}

	return image;
}

bool Image::checkInnerImage(int y, int x) {
	return (0 <= y
		&& y < Height
		&& 0 <= x
		&& x < Width);
}

void Image::drawPoint(MyPoint point, int size) {

	int radius = size / 2;

	for (int i = point.y - radius; i <= point.y + radius; i++) {
		for (int j = point.x - radius; j <= point.x + radius; j++) {
			if (checkInnerImage(i, j)) {
				B[i][j] = 0;
				G[i][j] = 0;
				R[i][j] = 255;
			}
		}
	}
}

void Image::drawLine(MyPoint point1, MyPoint point2) {
	if (point1 == point2)
		return;
	int startY;
	int startX;
	int endY;
	int endX;
	float angle = 0;

	if (point1.x == point2.x) {
		angle = M_PI_2;
		if (point1.y < point2.y) {
			startY = point1.y;
			startX = point1.x;
			endY = point2.y;
			endX = point2.x;
		}
		else {
			startY = point2.y;
			startX = point2.x;
			endY = point1.y;
			endX = point1.x;
		}
	}
	else {
		if (point1.x < point2.x) {
			startY = point1.y;
			startX = point1.x;
			endY = point2.y;
			endX = point2.x;
		}
		else {
			startY = point2.y;
			startX = point2.x;
			endY = point1.y;
			endX = point1.x;
		}
		float dy = startY - endY;
		float dx = startX - endX;
		angle = std::atanf(dy / dx);
	}

	float y = startY;
	float x = startX;

	while (1) {
		if (0 < y && y < Height
			&& 0 < x && x < Width) {
			B[(int)y][(int)x] = 0;
			G[(int)y][(int)x] = 255;
			R[(int)y][(int)x] = 255;
		}
		x += cosf(angle);
		y += sinf(angle);

		if (x < 0 || x >= Width || y < 0 || y >= Height)
			break;
		else if ((std::fabsf(x - endX) < 1) && (std::fabsf(y - endY) < 1))
			break;
	}
}

void Image::cutRect(MyPoint upLeft, MyPoint downRight) {
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			if (upLeft.x < j && j < downRight.x
				&& upLeft.y < i && i < downRight.y) {
			}
			else {
				B[i][j] = 255;
				G[i][j] = 255;
				R[i][j] = 255;
			}
		}

	}
}

void Image::expansion(Image* image, int count) {
	Image* buffImage = copy();

	for (int c = 0; c < count; c++) {
		for (int i = 0; i < buffImage->Height; i++) {
			for (int j = 0; j < buffImage->Width; j++) {
				if (buffImage->B[i][j] == 0
					&& buffImage->G[i][j] == 0
					&& buffImage->R[i][j] == 0) {
					int up = i - 1;
					if (up < 0) up = 0;
					int down = i + 1;
					if (down >= buffImage->Height) down = i;
					int right = j - 1;
					if (right < 0) right = 0;
					int left = j + 1;
					if (left >= buffImage->Width) left = j;

					if (buffImage->B[up][j] != 0
						|| buffImage->G[up][j] != 0
						|| buffImage->R[up][j] != 0
						|| buffImage->B[down][j] != 0
						|| buffImage->G[down][j] != 0
						|| buffImage->R[down][j] != 0
						|| buffImage->B[i][right] != 0
						|| buffImage->G[i][right] != 0
						|| buffImage->R[i][right] != 0
						|| buffImage->B[i][left] != 0
						|| buffImage->G[i][left] != 0
						|| buffImage->R[i][left] != 0) {
						B[i][j] = image->B[i][j];
						G[i][j] = image->G[i][j];
						R[i][j] = image->R[i][j];
					}
				}
			}
		}
		delete buffImage;
		buffImage = copy();
	}

	delete buffImage;
}

void Image::contraction(int count) {
	Image* buffImage = copy();

	for (int c = 0; c < count; c++) {
		for (int i = 0; i < buffImage->Height; i++) {
			for (int j = 0; j < buffImage->Width; j++) {
				if (buffImage->B[i][j] != 0
					&& buffImage->G[i][j] != 0
					&& buffImage->R[i][j] != 0) {
					int up = i - 1;
					if (up < 0) up = 0;
					int down = i + 1;
					if (down >= buffImage->Height) down = i;
					int right = j - 1;
					if (right < 0) right = 0;
					int left = j + 1;
					if (left >= buffImage->Width) left = j;

					if (
						(buffImage->B[up][j] == 0
							&& buffImage->G[up][j] == 0
							&& buffImage->R[up][j] == 0)
						|| (buffImage->B[down][j] == 0
							&& buffImage->G[down][j] == 0
							&& buffImage->R[down][j] == 0)
						|| (buffImage->B[i][right] == 0
							&& buffImage->G[i][right] == 0
							&& buffImage->R[i][right] == 0)
						|| (buffImage->B[i][left] == 0
							&& buffImage->G[i][left] == 0
							&& buffImage->R[i][left] == 0)) {
						B[i][j] = 0;
						G[i][j] = 0;
						R[i][j] = 0;
					}
				}
			}
		}
		delete buffImage;
		buffImage = copy();
	}

	delete buffImage;
}