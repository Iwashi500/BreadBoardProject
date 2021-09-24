#pragma once

class SchemDraw
{
private:
	CString fileName = "CircuitDiagram.py"; //生成する回路図表示用のpythonファイル名
	FILE* fp;

public:
	//描画するパーツの種類
	enum PartType
	{
		VDD,
		GND,
		Line,
		Resister,
		LED,
		Dot,
	};
	//描画する方向
	enum Direction {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	//インスタンスでファイルの読み込みとヘッダの書き込みを終わらせる
	SchemDraw() {
		//pythonファイルの作成
		errno_t error;
		if ((error = fopen_s(&fp, fileName, "wt, ccs=UTF-8")) != 0) {
			return;
		}

		//ヘッダ(インポート、Drawing生成)
		fputws(L"import schemdraw\n", fp);
		fputws(L"import schemdraw.elements as elm\n", fp);
		fputws(L"d = schemdraw.Drawing()\n", fp);
	}

	//フッタを書き込んで終了
	void draw(CStringW RESULT_PATH = L"") {
		//フッタ(表示、保存)
		fputws(L"d.draw()\n", fp);
		fputws(L"d.save('CircuitDiagram.svg')\n", fp);
		if (!RESULT_PATH.IsEmpty()) {
			CStringW saveW = L"d.save(r'" + RESULT_PATH + L"CircuitDiagram.svg')";
			const size_t newsizew = (saveW.GetLength() + 1) * 2;
			wchar_t* n2stringw = new wchar_t[newsizew];
			wcscpy_s(n2stringw, newsizew, saveW);
			fputws(n2stringw, fp);
		}

		fclose(fp);
	}
	void draw(CString RESULT_PATH) {
		draw((CStringW)RESULT_PATH);
	}

	void addVdd() {
		fputws(L"d += elm.VDD()\n", fp);
	}
	void addGND() {
		fputws(L"d += elm.GND()\n", fp);
	}
	void addDot() {
		fputws(L"d += elm.Dot()\n", fp);
	}
	void pop() {
		fputws(L"d.pop()\n", fp);
	}
	void push() {
		fputws(L"d.push()\n", fp);
	}
	//パーツの追加
	void addPart(PartType pt, Direction d = DOWN, CStringW label = L"");
};

