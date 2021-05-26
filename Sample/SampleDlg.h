
// SampleDlg.h : ヘッダー ファイル
//


#include "Image.h"
#include <vector>
#include <algorithm>
#include <list>
#include "BreadBoard.h"
#include <opencv2/opencv.hpp>
#pragma once

using namespace cv;

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

	CString IWI_PATH = "C:\\Users\\IWI\\Desktop\\IWI\\";
	CString RESULT_PATH = "C:\\Users\\IWI\\Desktop\\IWI\\結果\\";
	VideoCapture videoCapture;
	Mat input;
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
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	//const int WIDTH = 1920;
	//const int HEIGHT = 1080;

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
	afx_msg void OnHoleDetection();
	void getBoardRect(const Mat input, Rect& area);
};
