
// SampleDlg.h : ヘッダー ファイル
//


#include "Image.h"
#include <vector>
#include <algorithm>
#include <list>
#include "BreadBoard.h"
#include <opencv2/opencv.hpp>
#pragma once

//using namespace cv;

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
	unsigned char *m_OrgImage;
	BITMAPFILEHEADER m_OrgBmpFileHdr;
	LPBITMAPINFO m_OrgBmpInfo;
	CString m_fileName; //開いているファイルの名前
	CString m_filePath; //開いているファイルのパス

	CString IWI_PATH = "C:\\Users\\IWI\\Desktop\\IWI\\";
	CString RESULT_PATH = "C:\\Users\\IWI\\Desktop\\IWI\\結果\\";

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

public:
	afx_msg void OnOpen();
	void DrawImage();
	afx_msg void OnSave();
	afx_msg void OnBack();
	afx_msg void OnTest();
	void UpdateImage();
	bool SaveImage(Image* image, CString fileName);
	bool SaveImage(Image* image, CString fileName, CString pathName);
	Image* OpenImage();
	afx_msg void OnKido();
	afx_msg void OnEdgeSearch();
	bool checkInnerImage(int y, int x);
	afx_msg void OnChangeImageFormat();
	void drawPoint(MyPoint point, int size);
	void drawBoardPoints();
	void drawOnLine(OnLine line);
	void drawLine(MyPoint point1, MyPoint point2);
};
