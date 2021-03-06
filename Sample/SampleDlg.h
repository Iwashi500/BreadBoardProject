
// SampleDlg.h : ヘッダー ファイル
//


#include "Image.h"
#include <vector>
#include <algorithm>
#include <list>
#include "BreadBoard.h"
#include <opencv2/opencv.hpp>
#include "HoleType.h"
#include "PartLED.h"
#pragma once

using namespace cv;
using namespace std;

// CSampleDlg ダイアログ
class CSampleDlg : public CDialog
{
// コンストラクション
public:
	CSampleDlg(CWnd* pParent = nullptr);	// 標準コンストラクター
	//デストラクタ
	~CSampleDlg();

	//画像読み込みのためのメンバ変数の宣言
	unsigned char* m_BmpImage; //イメージデータを格納する変数
	LPBITMAPINFO m_BmpInfo; //BMPヘッダ情報を格納する変数
	BITMAPFILEHEADER m_BmpFileHdr; //BMPファイル情報を格納する変数
	Image* m_Image; //画像データを格納する変数
	Image* m_ChangeImage; //変更用の画像データ
	Image* m_BaseImage; //背景差分の比較元画像
	unsigned char *m_OrgImage;
	BITMAPFILEHEADER m_OrgBmpFileHdr;
	LPBITMAPINFO m_OrgBmpInfo;
	CString m_fileName; //開いているファイルの名前
	CString m_filePath; //開いているファイルのパス
	CString fileName = "CircuitDiagram.py"; //生成する回路図表示用のpythonファイル名

	CString IWI_PATH = "C:\\Users\\IWI\\Desktop\\IWI\\";
	CString RESULT_PATH = "C:\\Users\\IWI\\Desktop\\IWI\\結果\\";
	VideoCapture videoCapture;

	Mat input; //入力画像
	Mat concatInput; //上下線除去後に接合した入力画像
	int TOP_TO_LINT_DIS = 135; //ボードの頂点から、除去する線までの距離
	int LINE_HEIGHT = 50; //除去する線の高さ
	int MIDDLE_BOARD_HEIGHT = 635; //ボードの中央の高さ
	UINT m_timerID;

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BreadBoard breadBoard;
	//const int WIDTH = 1280;
	//const int HEIGHT = 720;
	const int WIDTH = 1920;
	const int HEIGHT = 1080;
	int fileIndex;

public:
	afx_msg void OnOpen();
	void DrawImage();
	afx_msg void OnSave();
	afx_msg void OnBack();
	afx_msg void OnTest();
	void UpdateImage();
	void UpdateImage(int height, int width);
	bool SaveImage(CString fileName);
	bool SaveImage(Image* image, CString fileName);
	bool SaveImage(Image* image, CString fileName, CString pathName);
	Image* OpenImage(CString fileName);
	afx_msg void OnKido();
	afx_msg void OnEdgeSearch();
	afx_msg void OnChangeImageFormat();
	void drawBoardPoints();
	void drawOnLine(OnLine line);
	afx_msg void OnGetImage();
	int getBITMAPINFO();
	int getBITMAPFILEHEADER();
	bool initCamera();
	afx_msg void OnCameraStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnChangeHSV();
	void cutImageUseHSV(Image* image);
	afx_msg void OnCameraStop();
	afx_msg void OnBaseDiff();
	afx_msg void OnSaveBaseBoard();
	void baseDiff();
	afx_msg void OnOpening();
	void showImage(Image* image);
	afx_msg void OnClosing();
	afx_msg void OnCreateCutBoard();
	afx_msg void OnHoukoku();
	void getBoardRect(const Mat input, Rect& area);
	void detectBoardHole(Mat input, Mat &result, Point leftTop, const Mat labels, const Mat status);
	void judgeHoleType(Mat input, Mat& result);
	HoleType saveHole(Point position);
	int getFileIndex() { return ++fileIndex;}
	void resetFileIndex() { fileIndex = 0; }
	void cutParts(Mat input, Mat& result, Mat mask, Mat labels, Mat status);
	void initSystem();
	Part* judgePartType(Mat input, int size, Rect area);
	void removeLEDTop(Rect roi, Rect area);
	void deleteResultFile();
	bool checkHoleUsed(Point& hole, Mat& result, Mat labels, Mat status);
	void reCheckHoleUsed(Mat& result, Mat labels, Mat status);
	void drawHoleType(Mat& result);
	void detectLineConnect();
	void drawLineConnect(Mat& result);
	void drawLineConnectInput(Mat& result, Point leftTop);
	void createTestBoard();
	void drawParts(Mat& result);
	void showCircuitDiagram();
	Connection* selectLEDAnode(PartLED* led, Point p1, Point p2);
	boolean judgeLEDAnode(PartLED* led, Point p1, Point p2, Connection* connect);
	afx_msg void OnCheckCircle();
	afx_msg void OnCheckAnswer();
	PartType selectTransistorType(Mat part);
	bool isPartTypeTransistor(PartType type);
	void saveImageResult(Mat image, String name);
	void saveImageTemp(Mat image, String name);
	bool isPartTypeIC(PartType type);
};
