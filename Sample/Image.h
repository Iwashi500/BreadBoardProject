#pragma once
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
};

