#include "pch.h"
#include "Image.h"

//�����t���R���X�g���N�^
Image::Image(int h, int w)
{
	//�����A��
	Height = h;
	Width = w;

	//�������G���[�΍�
	R = NULL;
	G = NULL;
	B = NULL;

	//�������m��
	R = new unsigned char* [Height];
	G = new unsigned char* [Height];
	B = new unsigned char* [Height];
	for (int i = 0; i < Height; i++) {
		R[i] = new unsigned char[Width];
		G[i] = new unsigned char[Width];
		B[i] = new unsigned char[Width];
	}
}

//�f�X�g���N�^
Image::~Image() {
	//�������J��
	if (R) {
		for (int i = 0; i < Height; i++) {
			delete[] R[i];
			delete[] G[i];
			delete[] B[i];
		}
	}
}