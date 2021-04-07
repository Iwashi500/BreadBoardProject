#include "pch.h"
#include "Image.h"

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
	}
}