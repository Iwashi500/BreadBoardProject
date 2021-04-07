#pragma once
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
};

