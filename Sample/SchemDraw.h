#pragma once

class SchemDraw
{
private:
	CString fileName = "CircuitDiagram.py"; //���������H�}�\���p��python�t�@�C����
	FILE* fp;

public:
	//�`�悷��p�[�c�̎��
	enum PartType
	{
		VDD,
		GND,
		Line,
		Resister,
		LED,
		Dot,
	};
	//�`�悷�����
	enum Direction {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	//�C���X�^���X�Ńt�@�C���̓ǂݍ��݂ƃw�b�_�̏������݂��I��点��
	SchemDraw() {
		//python�t�@�C���̍쐬
		errno_t error;
		if ((error = fopen_s(&fp, fileName, "wt, ccs=UTF-8")) != 0) {
			return;
		}

		//�w�b�_(�C���|�[�g�ADrawing����)
		fputws(L"import schemdraw\n", fp);
		fputws(L"import schemdraw.elements as elm\n", fp);
		fputws(L"d = schemdraw.Drawing()\n", fp);
	}

	//�t�b�^����������ŏI��
	void draw(CStringW RESULT_PATH = L"") {
		//�t�b�^(�\���A�ۑ�)
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
	//�p�[�c�̒ǉ�
	void addPart(PartType pt, Direction d = DOWN, CStringW label = L"");
};

