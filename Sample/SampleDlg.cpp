
// SampleDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "Sample.h"
#include "SampleDlg.h"
#include "SchemDraw.h"
#include "afxdialogex.h"
#include "PartType.h"
#include "PartLED.h"
#include "PartSwitch.h"
#include "PartCondenser.h"
#include "PartTransistor.h"
#include "PartIC8.h"
#include "ConnectionTransistor.h"
#include "iostream"
#include <time.H>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <list>
#include "LocalConfig.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <atlpath.h> // CPathのため必要
#include <iostream>
#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>

#define BPP 24
#define ICEIL(dividend, divisor) (((dividend) + ((divisor) - 1)) / (divisor))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//色
#define BLUE Scalar(255, 0, 0)
#define GREEN Scalar(0, 255, 0)
#define RED Scalar(0, 0, 255)
#define YELLOW Scalar(0, 255, 255)
#define PURPLE Scalar(128, 0, 128);
#define LIGHTBLUE Scalar(255, 255, 0);
#define WHITE Scalar(255, 255, 255)
#define ORANGE Scalar(0, 165, 255);
#define GRAY Scalar(165, 165, 165);
#define PINK Scalar(255, 0, 255);

using namespace cv;
using namespace std;

struct mouseParam
{
	int x;
	int y;
	int event;
	int flags;
};

void callBackMouseFunc(int eventType, int x, int y, int flags, void* userdata) {
	mouseParam* ptr = static_cast<mouseParam*> (userdata);

	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSampleDlg ダイアログ


//コンストラクタ
CSampleDlg::CSampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SAMPLE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//メモリエラー対策
	m_BmpImage = NULL;
	m_BmpInfo = NULL;
	m_BmpFileHdr = {};
	m_Image = NULL;
	m_OrgImage = NULL;
	m_OrgBmpFileHdr = {};
	m_OrgBmpInfo = NULL;
	m_ChangeImage = NULL;
	m_BaseImage = NULL;

	//LocalConfigクラスの作成が必要(Local設定ファイルのため.gitignore)
	IWI_PATH = LocalConfig::saveBasePath();
	RESULT_PATH = LocalConfig::saveResultPath();
	breadBoard = BreadBoard();
	fileIndex = 0;
}

//デストラクタ
CSampleDlg::~CSampleDlg()
{
	//メモリ開放
	if (m_BmpImage) {
		delete[] m_BmpImage;
	}

	if (m_Image) {
		delete[] m_Image;
	}
	if (m_ChangeImage) {
		delete[] m_ChangeImage;
	}
	if (m_BaseImage) {
		delete[] m_BaseImage;
	}

	if (m_OrgImage)
		delete[] m_OrgImage;

	cv::destroyAllWindows();
}


void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &CSampleDlg::OnOpen)
	ON_BN_CLICKED(IDC_BUTTON2, &CSampleDlg::OnSave)
	ON_BN_CLICKED(IDC_BUTTON1, &CSampleDlg::OnBack)
	ON_BN_CLICKED(IDC_BUTTON8, &CSampleDlg::OnTest)
	ON_BN_CLICKED(IDC_BUTTON3, &CSampleDlg::OnKido)
	ON_BN_CLICKED(IDC_BUTTON4, &CSampleDlg::OnEdgeSearch)
	ON_BN_CLICKED(IDC_BUTTON5, &CSampleDlg::OnChangeImageFormat)
	ON_BN_CLICKED(IDC_BUTTON6, &CSampleDlg::OnGetImage)
	ON_BN_CLICKED(IDC_BUTTON7, &CSampleDlg::OnCameraStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON9, &CSampleDlg::OnChangeHSV)
	ON_BN_CLICKED(IDC_BUTTON10, &CSampleDlg::OnCameraStop)
	ON_BN_CLICKED(IDC_BUTTON11, &CSampleDlg::OnBaseDiff)
	ON_BN_CLICKED(IDC_BUTTON12, &CSampleDlg::OnSaveBaseBoard)
	ON_BN_CLICKED(IDC_BUTTON13, &CSampleDlg::OnOpening)
	ON_BN_CLICKED(IDC_BUTTON14, &CSampleDlg::OnClosing)
	ON_BN_CLICKED(IDC_BUTTON16, &CSampleDlg::OnCreateCutBoard)
	ON_BN_CLICKED(IDC_BUTTON15, &CSampleDlg::OnHoukoku)
	ON_BN_CLICKED(IDC_BUTTON37, &CSampleDlg::OnCheckCircle)
	ON_BN_CLICKED(IDC_BUTTON38, &CSampleDlg::OnCheckAnswer)
END_MESSAGE_MAP()


// CSampleDlg メッセージ ハンドラー

BOOL CSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	//画像描画関数
	DrawImage();
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSampleDlg::OnOpen()
{
	//BMPファイルの読み込み
	//変数の宣言
	int i, j;
	size_t image_size;
	CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;

	//----------------------------ファイル操作ダイアログの呼び出し処理

	//ファイルダイアログを表示してファイル指定する
	CFileDialog myDLG(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");
	if (myDLG.DoModal() != IDOK)return;

	//取得したファイルをオープンする
	filename = myDLG.GetPathName();
	if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
		return;
	}
	int len = filename.ReverseFind('\\');
	m_fileName = filename.Right(filename.GetLength() - len - 1);
	m_fileName = m_fileName.Left(m_fileName.GetLength() - 4);
	m_filePath = filename.Left(filename.GetLength() - m_fileName.GetLength() - 4);

	//タイトルバーにファイル名を表示する
	SetWindowText("VCtest" + filename);

	//----------------------------------------------ファイル内容の読み込み処理

	//ファイルヘッダ部とインフォヘッダ部を読み込む
	file.Read(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	//前回の画像イメージをいったん解放
	if (m_BmpImage)delete[] m_BmpImage;

	//前回使用していれば、いったんメモリ解放する
	if (m_BmpInfo)delete[] m_BmpInfo;

	//前回の画像処理メモリをいったん解放
	if (m_Image)delete[] m_Image;

	if (m_OrgImage)delete[] m_OrgImage;

	//色情報を取得する
	//biBitCountは1ピクセルあたりのカラー表現ビット数
	//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//16ビット以上と未満でカラーデータ格納が異なる
	if (myBmpInfoHdr.biBitCount >= 16) {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(m_BmpInfo->bmiColors,
			1i64 << (myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}

	//m_BmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	m_BmpInfo->bmiHeader = myBmpInfoHdr;

	//ファイル内のビットマップ実データ位置に合わせる
	file.Seek(m_BmpFileHdr.bfOffBits, CFile::begin);

	//実データ分のバイト数を確保
	//bfsize	:ビットマップファイル全サイズ
	//bfffBits	:先頭にあるヘッダ情報サイズ
	image_size = m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits;
	m_BmpImage = new unsigned char[image_size];

	//ビットマップ実データを読み込み格納
	file.Read(m_BmpImage, (UINT)image_size);

	file.Close();

	//画像処理用のメモリを動的に確保
	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	m_ChangeImage = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {//フルカラーの場合
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
				m_Image->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				m_Image->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
				m_ChangeImage->B[i][j] = m_Image->B[i][j];
				m_ChangeImage->G[i][j] = m_Image->G[i][j];
				m_ChangeImage->R[i][j] = m_Image->R[i][j];
			}
		}
	}
	else {
	}

	//-----------------------------------------------戻る用にコピー
	//メモリの確保
	m_OrgImage = new unsigned char[image_size];
	//メモリのコピー
	memcpy(m_OrgImage, m_BmpImage, image_size);
	//ファイルヘッダ、インフォの保存
	m_OrgBmpFileHdr = m_BmpFileHdr;
	m_OrgBmpInfo = m_BmpInfo;


	//-----------------------------------------------再描画指示

	//再描画支持
	Invalidate();
}

void CSampleDlg::showImage(Image* image) {
	CString filePath = "./";
	CString name = "test";
	CString fileName = filePath + name + ".bmp";
	SaveImage(image, "test", "./");

	int i, j;
	size_t image_size;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;

	//----------------------------ファイル操作ダイアログの呼び出し処理
	//取得したファイルをオープンする
	if (!file.Open(fileName, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

	//----------------------------------------------ファイル内容の読み込み処理

	//ファイルヘッダ部とインフォヘッダ部を読み込む
	file.Read(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	//前回の画像イメージをいったん解放
	if (m_BmpImage)delete[] m_BmpImage;

	//前回使用していれば、いったんメモリ解放する
	if (m_BmpInfo)delete[] m_BmpInfo;

	//前回の画像処理メモリをいったん解放
	if (m_Image)delete[] m_Image;

	if (m_OrgImage)delete[] m_OrgImage;

	//色情報を取得する
	//biBitCountは1ピクセルあたりのカラー表現ビット数
	//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//16ビット以上と未満でカラーデータ格納が異なる
	if (myBmpInfoHdr.biBitCount >= 16) {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(m_BmpInfo->bmiColors,
			1i64 << (myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}

	//m_BmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	m_BmpInfo->bmiHeader = myBmpInfoHdr;

	//ファイル内のビットマップ実データ位置に合わせる
	file.Seek(m_BmpFileHdr.bfOffBits, CFile::begin);

	//実データ分のバイト数を確保
	//bfsize	:ビットマップファイル全サイズ
	//bfffBits	:先頭にあるヘッダ情報サイズ
	image_size = m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits;
	m_BmpImage = new unsigned char[image_size];

	//ビットマップ実データを読み込み格納
	file.Read(m_BmpImage, (UINT)image_size);
	file.Close();

	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	m_ChangeImage = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {//フルカラーの場合
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
				m_Image->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				m_Image->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
				m_ChangeImage->B[i][j] = m_Image->B[i][j];
				m_ChangeImage->G[i][j] = m_Image->G[i][j];
				m_ChangeImage->R[i][j] = m_Image->R[i][j];
			}
		}
	}
	else {
	}

	//-----------------------------------------------戻る用にコピー
	//メモリの確保
	m_OrgImage = new unsigned char[image_size];
	//メモリのコピー
	memcpy(m_OrgImage, m_BmpImage, image_size);
	//ファイルヘッダ、インフォの保存
	m_OrgBmpFileHdr = m_BmpFileHdr;
	m_OrgBmpInfo = m_BmpInfo;

	Invalidate();
}

void CSampleDlg::DrawImage()
{
	//画像描画処理
	HBITMAP myDIB; //ビットマップのハンドル
	HBITMAP oldDIB;
	HDC hDC; //デバイスコンテキストのハンドル
	HDC hCompatiDC; //メモリデバイスコンテキスト
	PAINTSTRUCT ps;  //クライアント領域の描画に使う情報を保持

	CBitmap myBMP;

	//画像の準備ができていなければリターン
	if (!m_BmpInfo) return;

	//描画の準備をする
	//hDC = ::GetDC(m_hWnd);
	hDC = ((CStatic*)GetDlgItem(IDC_PICBOX))->GetDC()->GetSafeHdc();

	//描画の準備をする
	//描画が終了したら必ずEndPaintを実行する必要がある
	::BeginPaint(m_hWnd, &ps);

	//hDCと互換性のあるメモリデバイスコンテキストを作成する
	hCompatiDC = ::CreateCompatibleDC(hDC);

	//DIB形式ビットマップからDDB（デバイス依存）形式ビットマップを作成する
	myDIB = ::CreateDIBitmap(hDC, &m_BmpInfo->bmiHeader,
		CBM_INIT, m_BmpImage, m_BmpInfo, DIB_RGB_COLORS);

	//hCompatiDCでmyDIBの示すオブジェクトを使用できるようにする
	//オリジナルビットマップをoldDIBに保存しておき後で復活させる
	oldDIB = (HBITMAP)::SelectObject(hCompatiDC, myDIB);

	//hCOmpatiDCにある画像をhDCに表示する
	BitBlt(hDC, 0, 0, m_BmpInfo->bmiHeader.biWidth, m_BmpInfo->bmiHeader.biHeight,
		hCompatiDC, 0, 0, SRCCOPY);

	//BeginPaint対応処理
	::EndPaint(m_hWnd, &ps);

	//オブジェクトの解放など
	::SelectObject(hCompatiDC, oldDIB);
	::DeleteObject(myDIB);
	::DeleteObject(hCompatiDC);
	::ReleaseDC(m_hWnd, hDC);

}

void CSampleDlg::OnSave()
{
	//画像保存処理
	CString filename;
	CFile file;

	//フルカラー以外の場合は省略（減色処理が必要になるので）
	if (m_BmpInfo->bmiHeader.biBitCount != 24) {
		MessageBox("フルカラーでない画像は保存できません", "がんばれ", MB_OK);
		return;
	}

	//ファイルダイアログを表示してファイル指定する
	CFileDialog myDLG(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");

	if (myDLG.DoModal() != IDOK)return;

	//取得したファイルをオープンする
	filename = myDLG.GetPathName() + ".bmp";

	if (!file.Open(filename, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary)) {
		MessageBox((CString)"ファイル" + filename + "の書き込みに失敗しました", "エラー", MB_OK);
		return;
	}

	//ファイル内容の書き込み
	file.Write(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Write(&m_BmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
	file.Write(m_BmpImage, m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits);

	file.Close();
}

bool CSampleDlg::SaveImage(Image* image, CString fileName) {
	return SaveImage(image, fileName, RESULT_PATH);
}

//m_Image保存で固定
bool CSampleDlg::SaveImage(CString fileName) {
	fileName += ".bmp";
	//画像保存処理
	CFile file;

	//フルカラー以外の場合は省略（減色処理が必要になるので）
	if (m_BmpInfo->bmiHeader.biBitCount != 24) {
		MessageBox("フルカラーでない画像は保存できません", "がんばれ", MB_OK);
		return false;
	}

	if (!file.Open(fileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary)) {
		MessageBox((CString)"ファイル" + fileName + "の書き込みに失敗しました", "エラー", MB_OK);
		return false;
	}

	//ファイル内容の書き込み
	file.Write(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Write(&m_BmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
	file.Write(m_BmpImage, m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits);

	file.Close();

	return true;
}

bool CSampleDlg::SaveImage(Image* image, CString fileName, CString pathName) {
	//画像保存処理
	//第8回_01追加分：拡大・縮小後のサイズに変更
	int exWidth, exHeight;
	size_t ExImageSize;
	BITMAPFILEHEADER bmpFileHdr = m_BmpFileHdr;
	LPBITMAPINFO bmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];

	exWidth = (int)(image->Width);   //拡大・縮小後の幅
	exHeight = (int)(image->Height); //拡大・縮小後の高さ

	//BMPの場合幅を4の倍数にする必要があるため4の倍数になるようにする
	exWidth = exWidth - exWidth % 4;
	exHeight = exHeight - exHeight % 4;

	DWORD bytesPerPixel = BPP / 8; //1画素当たりのバイト数

	//走査線の正味のバイト数を計算する
	DWORD lineSizeDW = bytesPerPixel * exWidth;

	//"lineSizeDW"を実際のサイズ(DWORD 数)にするため，sizeof(DWORD)で割る(小数部切り上げ)
	lineSizeDW = ICEIL(lineSizeDW, sizeof(DWORD));

	DWORD lineSize = lineSizeDW * sizeof(DWORD);
	ExImageSize = lineSize * exHeight;

	//拡大・縮小後の値を格納
	bmpInfo->bmiHeader.biWidth = (long)exWidth;
	bmpInfo->bmiHeader.biHeight = (long)exHeight;
	bmpInfo->bmiHeader.biSizeImage = (DWORD)ExImageSize;
	bmpInfo->bmiHeader.biBitCount = m_BmpInfo->bmiHeader.biBitCount;
	bmpInfo->bmiHeader.biSizeImage = ExImageSize;
	bmpInfo->bmiHeader.biClrImportant = m_BmpInfo->bmiHeader.biClrImportant;
	bmpInfo->bmiHeader.biClrUsed = m_BmpInfo->bmiHeader.biClrUsed;
	bmpInfo->bmiHeader.biCompression = m_BmpInfo->bmiHeader.biCompression;
	bmpInfo->bmiHeader.biPlanes = m_BmpInfo->bmiHeader.biPlanes;
	bmpInfo->bmiHeader.biSize = m_BmpInfo->bmiHeader.biSize;
	bmpInfo->bmiHeader.biXPelsPerMeter = m_BmpInfo->bmiHeader.biXPelsPerMeter;
	bmpInfo->bmiHeader.biYPelsPerMeter = m_BmpInfo->bmiHeader.biYPelsPerMeter;
	bmpFileHdr.bfSize = bmpInfo->bmiHeader.biSizeImage + m_BmpFileHdr.bfOffBits;
	size_t image_size = bmpFileHdr.bfSize - bmpFileHdr.bfOffBits;
	unsigned char* bmpImage = new unsigned char[image_size];
	for (int i = 0; i < exHeight; i++) {
		for (int j = 0; j < exWidth; j++) {
			bmpImage[((bmpInfo->bmiHeader.biHeight - i - 1) * bmpInfo->bmiHeader.biWidth + j) * 3] = image->B[i][j];
			bmpImage[((bmpInfo->bmiHeader.biHeight - i - 1) * bmpInfo->bmiHeader.biWidth + j) * 3 + 1] = image->G[i][j];
			bmpImage[((bmpInfo->bmiHeader.biHeight - i - 1) * bmpInfo->bmiHeader.biWidth + j) * 3 + 2] = image->R[i][j];
		}
	}

	//画像保存処理
	CFile file;

	//フルカラー以外の場合は省略（減色処理が必要になるので）
	/*if (bmpInfo->bmiHeader.biBitCount != 24) {
		MessageBox("フルカラーでない画像は保存できません", "がんばれ", MB_OK);
		return false;
	}*/

	//取得したファイルをオープンする
	fileName = pathName + fileName + ".bmp";

	if (!file.Open(fileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary)) {
		MessageBox((CString)"ファイル" + fileName + "の書き込みに失敗しました", "エラー", MB_OK);
		return false;
	}

	//ファイル内容の書き込み
	file.Write(&bmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Write(&bmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
	file.Write(bmpImage, bmpFileHdr.bfSize - bmpFileHdr.bfOffBits);

	file.Close();

	return true;
}

void CSampleDlg::OnBack()
{
	int i, j;

	//ビットマップファイルヘッダ・インフォ元画像に戻す。
	m_BmpFileHdr = m_OrgBmpFileHdr;
	m_BmpInfo = m_OrgBmpInfo;

	//元画像のメモリを表示用画像にコピーする。
	memcpy(m_BmpImage, m_OrgImage, m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits);

	for (i = 0; i < m_Image->Height; i++) {
		for (j = 0; j < m_Image->Width; j++) {
			m_Image->B[i][j] = m_ChangeImage->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
			m_Image->G[i][j] = m_ChangeImage->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
			m_Image->R[i][j] = m_ChangeImage->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
		}
	}

	//再描画
	Invalidate();
}

void CSampleDlg::drawBoardPoints() {
	//std::list<MyPoint> points;
	//MyPoint base(54, 20);
	//MyPoint add(8, 8);

	//for (int j = 0; j < 63; j++) {
	//	OnLine onLine;
	//	ChainPoint chain(base, MyPoint(base.y, base.x + add.x * j));
	//	for (int i = 0; i < 5; i++) {
	//		int y = base.y + add.y * i;
	//		int x = base.x + add.x * j;
	//		MyPoint point(y, x);
	//		m_Image->drawPoint(point, 3);
	//		chain.pushNext(point);
	//		onLine.push(chain);
	//	}
	//	breadBoard.push(onLine);
	//}

	//base.y = 110;
	//for (int j = 0; j < 63; j++) {
	//	OnLine onLine;
	//	ChainPoint chain(base, MyPoint(base.y, base.x + add.x * j));
	//	for (int i = 0; i < 5; i++) {
	//		int y = base.y + add.y * i;
	//		int x = base.x + add.x * j;
	//		MyPoint point(y, x);
	//		m_Image->drawPoint(point, 3);
	//		chain.pushNext(point);
	//		onLine.push(chain);
	//	}
	//	breadBoard.push(onLine);
	//}

	//for (OnLine onLine : breadBoard.getOnLines()) {
	//	drawOnLine(onLine);
	//}

	////base.y = 24;
	////base.x = 36;
	////for (int i = 0; i < 2; i++) {
	////	for (int j = 0; j < 59; j++) {
	////		int y = base.y + add.y * i;
	////		int x = base.x + add.x * j;
	////		drawPoint(MyPoint(y, x), 3);
	////	}
	////}

	////base.y = 165;
	////for (int i = 0; i < 2; i++) {
	////	for (int j = 0; j < 59; j++) {
	////		int y = base.y + add.y * i;
	////		int x = base.x + add.x * j;
	////		drawPoint(MyPoint(y, x), 3);
	////	}
	////}

	//UpdateImage();std::list<MyPoint> points;
	//MyPoint base(54, 20);
	//MyPoint add(8, 8);

	//for (int j = 0; j < 63; j++) {
	//	OnLine onLine;
	//	ChainPoint chain(base, MyPoint(base.y, base.x + add.x * j));
	//	for (int i = 0; i < 5; i++) {
	//		int y = base.y + add.y * i;
	//		int x = base.x + add.x * j;
	//		MyPoint point(y, x);
	//		m_Image->drawPoint(point, 3);
	//		chain.pushNext(point);
	//		onLine.push(chain);
	//	}
	//	breadBoard.push(onLine);
	//}

	//base.y = 110;
	//for (int j = 0; j < 63; j++) {
	//	OnLine onLine;
	//	ChainPoint chain(base, MyPoint(base.y, base.x + add.x * j));
	//	for (int i = 0; i < 5; i++) {
	//		int y = base.y + add.y * i;
	//		int x = base.x + add.x * j;
	//		MyPoint point(y, x);
	//		m_Image->drawPoint(point, 3);
	//		chain.pushNext(point);
	//		onLine.push(chain);
	//	}
	//	breadBoard.push(onLine);
	//}

	//for (OnLine onLine : breadBoard.getOnLines()) {
	//	drawOnLine(onLine);
	//}

	////base.y = 24;
	////base.x = 36;
	////for (int i = 0; i < 2; i++) {
	////	for (int j = 0; j < 59; j++) {
	////		int y = base.y + add.y * i;
	////		int x = base.x + add.x * j;
	////		drawPoint(MyPoint(y, x), 3);
	////	}
	////}

	////base.y = 165;
	////for (int i = 0; i < 2; i++) {
	////	for (int j = 0; j < 59; j++) {
	////		int y = base.y + add.y * i;
	////		int x = base.x + add.x * j;
	////		drawPoint(MyPoint(y, x), 3);
	////	}
	////}

	//UpdateImage();
}

void CSampleDlg::initSystem() {
	resetFileIndex();
	breadBoard = BreadBoard();
	cv::destroyAllWindows();
}

bool deleteFileInDirectory(CString directoryPath) {
	// 対象のディレクトリ
	CFileFind find;
	CPath searchPath(directoryPath);
	searchPath.Append(_T("*"));

	// 対象のディレクトリがない場合、終了します。
	if (!find.FindFile(searchPath, 0)) return TRUE;

	// 対象のディレクトリ内のファイルまたはディレクトリが存在する間繰り返します。
	BOOL result = FALSE;
	do
	{
		// ディレクトリ内のファイルまたはディレクトリを取得します。
		result = find.FindNextFile();

		// "."または".."の場合、次を処理します。
		if (find.IsDots()) continue;

		// 対象のパスを取得します。
		CPath targetPath(directoryPath);
		targetPath.Append(find.GetFileName());

		// 対象のパスがディレクトリの場合、ディレクトリ内を再帰的に処理します。
		if (find.IsDirectory()) deleteFileInDirectory(targetPath);

		// 対象のパスがファイルの場合、削除します。
		else ::DeleteFile(targetPath);
	} while (result);
	find.Close();

	return true;
}

void CSampleDlg::deleteResultFile() {
	CString path = RESULT_PATH;

	deleteFileInDirectory(path + "holes");
	deleteFileInDirectory(path + "parts");
	deleteFileInDirectory(path + "temp");
}

void CSampleDlg::drawHoleType(Mat& result) {
	for (int i = 0; i < 14; i++) {
		int row = 30;
		if (i < 2 || 11 < i)
			row = 25;
		for (int j = 0; j < row; j++) {
			Point hole = breadBoard.holePositions.at(i).at(j);
			int s = 10 * 1.5;
			int left = hole.x - s;
			int top = hole.y - s;
			int right = hole.x + s;
			int down = hole.y + s;

			HoleType type = breadBoard.holeTypes.at(i).at(j);
			Scalar color;
			if (type.type == HoleType::MIDDLE)
				color = Scalar(255, 255, 0);
			else if (type.type == HoleType::EDGE)
				color = Scalar(0, 255, 255);
			else if (type.type == HoleType::EMPTY) 
				color = Scalar(0, 255, 0);

			rectangle(result, Rect(left, top, s * 2, s * 2), color, 2);	
		}
	}
}

void CSampleDlg::OnCheckCircle()
{
	//抵抗のみ直線
	//breadBoard.connections.push_back(
	//	Connection(Point(7, 1), Point(8, 2), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(8, 8), PartType::RESISTOR));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 8), Point(7, 12), PartType::WIRE));

	//LEDつき分岐なし
	//breadBoard.connections.push_back(
	//	Connection(Point(7, 1), Point(8, 2), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(8, 8), PartType::RESISTOR));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 8), Point(8, 10), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 10), Point(12, 10), PartType::LED));
	//breadBoard.connections.push_back(
	//	Connection(Point(12, 10), Point(10, 12), PartType::WIRE));

	//VCC接続無し
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(8, 8), PartType::RESISTOR));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 8), Point(8, 10), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 10), Point(12, 10), PartType::LED));
	//breadBoard.connections.push_back(
	//	Connection(Point(12, 10), Point(10, 12), PartType::WIRE));

	////銅線のみショート
	//breadBoard.connections.push_back(
	//	Connection(Point(7, 1), Point(8, 2), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(8, 8), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 8), Point(7, 12), PartType::WIRE));

	//スイッチ有ショート
	//breadBoard.connections.push_back(
	//	Connection(Point(7, 1), Point(8, 2), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(10, 7), PartType::SWITCH));
	//breadBoard.connections.push_back(
	//	Connection(Point(10, 7), Point(9, 12), PartType::WIRE));

	//スイッチ・LEDショート
	//breadBoard.connections.push_back(
	//	Connection(Point(7, 1), Point(8, 2), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::LED));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(10, 7), PartType::SWITCH));
	//breadBoard.connections.push_back(
	//	Connection(Point(10, 7), Point(9, 12), PartType::WIRE));


	//途切れあり
	//breadBoard.connections.push_back(
	//	Connection(Point(7, 1), Point(8, 2), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 2), Point(8, 7), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 7), Point(8, 8), PartType::RESISTOR));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 8), Point(7, 12), PartType::WIRE));
	//breadBoard.connections.push_back(
	//	Connection(Point(8, 8), Point(10, 8), PartType::WIRE));

	BreadBoard::ErrorType et = breadBoard.checkCircleError();

	switch (et)
	{
	case BreadBoard::NONE:
		MessageBox(TEXT("回路は安全です。"), TEXT("OK"), MB_OK);
		break;
	case BreadBoard::SHORT:
		MessageBox(TEXT("ショートしています。\n電源を接続しないでください。"), TEXT("危険！！！"), MB_OK);
		break;
	case BreadBoard::USELESS:
		MessageBox(TEXT("どこにも接続されていない線があります。\n回路を確認してください。"), TEXT("注意"), MB_OK);
		break;
	case BreadBoard::NO_VCC:
		MessageBox(TEXT("VCCに接続されていません。\n回路を確認してください。"), TEXT("注意"), MB_OK);
		break;
	case BreadBoard::NO_CONDUCTION:
		MessageBox(TEXT("導通している線がありません。\n回路を確認してください。"), TEXT("注意"), MB_OK);
	default:
		break;
	}
}

void CSampleDlg::OnCheckAnswer()
{
	//MessageBox(TEXT("正解の回路と同一です。"), TEXT("OK"), MB_OK);
	MessageBox(TEXT("正解の回路と異なる箇所があります。\n回路を確認してください。"), TEXT("注意"), MB_OK);
}

void CSampleDlg::OnTest()
{	
	deleteResultFile();
	initSystem();
	String path = RESULT_PATH;

	bool useCamera = false;

	/*if (useCamera && !videoCapture.isOpened())
		initCamera();*/
	videoCapture.read(input);
	if (input.empty()) {
		String inputPath = RESULT_PATH + "input.bmp";
		input = imread(inputPath, 1);
	
		if (input.size == 0)
			return;
	}

	Mat result;

	//上下線の領域取得
	Point minP, maxP;
	Rect boardArea;
	getBoardRect(input, boardArea);

	//ボード領域マスク画像生成
	Mat boardMask(Size(input.cols, input.rows), CV_8UC1, Scalar(0));
	rectangle(boardMask, boardArea, 255, -1);

	//入力画像の上下線を除去する
	Rect topLine = boardArea;
	Rect downLine = boardArea;
	Mat removeResult;
	topLine.y += TOP_TO_LINT_DIS;
	topLine.height = LINE_HEIGHT;
	downLine.y = boardArea.y + boardArea.height - TOP_TO_LINT_DIS - LINE_HEIGHT;
	downLine.height = LINE_HEIGHT;
	rectangle(boardMask, topLine, 0, -1);
	rectangle(boardMask, downLine, 0, -1);
	input.copyTo(removeResult, boardMask);

	//上下線を除去して結合
	Rect t1r = boardArea;
	t1r.height = topLine.y - boardArea.y;
	Rect t2r = boardArea;
	t2r.y = topLine.y + topLine.height;
	t2r.height = downLine.y - t2r.y;
	Rect t3r = t1r;
	t3r.y = downLine.y + downLine.height;
	Mat t1 = Mat(input, t1r);
	Mat t2 = Mat(input, t2r);
	Mat t3 = Mat(input, t3r);

	Mat tes[3] = { t1, t2, t3 };
	vconcat(tes, 3, concatInput);

	//Rect t4r = t2r;
	//int h = 270;
	//t4r.height = h;
	//Rect t5r = t4r;
	//t5r.y = t2r.y + h + 60;
	//Mat t4 = Mat(input, t4r);
	//Mat t5 = Mat(input, t5r);

	//Mat tes[4] = { t1, t4, t5, t3 };
	//vconcat(tes, 4, concatInput);

	//HSV
	Mat hsv;
	cvtColor(concatInput, hsv, CV_BGR2HSV);
	Scalar sMin = Scalar(0, 0, 110);
	Scalar sMax = Scalar(180, 20, 255);
	Mat mask;
	inRange(hsv, sMin, sMax, mask);

	//収縮処理
	Mat erosion;
	morphologyEx(mask, erosion, MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(5, 5)));
	//erosion = mask.clone();

	//反転
	Mat reverse;
	cv::bitwise_not(erosion, reverse);
	//Mat buf;
	//reverse.copyTo(buf, boardMask);
	//reverse = buf.clone();

	//ラベリング
	Mat labels, stats, centroids;
	int nLab = connectedComponentsWithStats(reverse, labels, stats, centroids, 4, CV_32S);
	Mat filLabels = labels.clone();
	int borderMax = 600;
	int borderMin = 90;
	Mat filter;
	reverse.copyTo(filter);
	Mat output(labels.size(), labels.type());
	Mat_<float> input_1b = Mat_<float>(labels);
	Mat_<float> labels_1b = Mat_<float>(output);
	int x = labels_1b.size().width;
	int y = labels_1b.size().height;
	for (int i = 0; i < y; ++i) {
		for (int j = 0; j < x; ++j) {
			float pixel = input_1b(i, j);
			int* label = filLabels.ptr<int>(i, j);
			int* param = stats.ptr<int>(*label);
			int size = param[ConnectedComponentsTypes::CC_STAT_AREA];
			if (*label != 0 && borderMin < size && size < borderMax)
				pixel = 255;
			else {
				pixel = 0;
				*label = 0;
			}
			filter.at<unsigned char>(i, j) = pixel;
		}
	}

	//搭載穴の左上検出
	Mat hole;
	concatInput.copyTo(hole, filter);
	//hole.copyTo(hole, boardMask);
	Point leftTop(boardArea.x, boardArea.y);

	//搭載穴の検出
	Mat holeDetect;
	detectBoardHole(hole, holeDetect, Point(0, 0), filLabels, stats);

	//搭載穴の判別
	Mat ht;
	judgeHoleType(holeDetect, ht);

	//穴の状態の画像生成
	Mat holeType = concatInput.clone();
	drawHoleType(holeType);

	//部品の抜き出し
	Mat parts;
	cutParts(concatInput, parts, reverse, labels, stats);

	drawParts(parts);

	//配線検出
	Mat lineConnect = concatInput.clone();
	detectLineConnect();
	drawLineConnect(lineConnect);

	////結果画像生成
	//最終の搭載穴判定画像
	Mat holeTypeResult = concatInput.clone();
	drawHoleType(holeTypeResult);
	//input画像への配線検出結果
	Mat lineConnectInput = input.clone();
	drawLineConnectInput(lineConnectInput, Point(boardArea.x, boardArea.y));

	Mat showResult;
	resize(lineConnectInput, showResult, Size(), 0.5, 0.5);

	//回路図の表示
	showCircuitDiagram();
	Mat CircuitDiagram = imread("CircuitDiagram.png");

	//回路のチェック
	imshow("result", showResult);
	imshow("circuit", CircuitDiagram);
	//OnCheckCircle();

	//画像保存
	saveImageResult(input, "input");
	saveImageResult(boardMask, "holeMask");
	saveImageResult(removeResult, "removeResult");
	saveImageResult(concatInput, "concat");
	saveImageResult(hsv, "hsv");
	saveImageResult(mask, "mask");
	saveImageResult(erosion, "erosion");
	saveImageResult(reverse, "reverse");
	saveImageResult(labels, "labels");
	saveImageResult(filLabels, "filLabels");
	saveImageResult(filter, "filter");
	saveImageResult(hole, "hole");
	saveImageResult(holeDetect, "holeDetect");
	saveImageResult(holeType, "holeType");
	saveImageResult(lineConnect, "lineConnect");
	saveImageResult(holeTypeResult, "holeTypeResult");
	saveImageResult(lineConnectInput, "lineConnectInput");
	saveImageResult(CircuitDiagram, "CircuitDiagram");
}

void CSampleDlg::saveImageResult(Mat image, String name) {
	String path = RESULT_PATH;
	imwrite(path + format("%d_", getFileIndex()) + name + ".bmp", image);
}

void CSampleDlg::saveImageTemp(Mat image, String name) {
	String path = RESULT_PATH + "temp\\";
	imwrite(path +  name + ".bmp", image);
}

void CSampleDlg::showCircuitDiagram() {
	//CSVに保存
	CString fileName = "connects-parameter.csv"; //生成する回路図表示用のpythonファイル名
	FILE* fp;
	errno_t error;
	if ((error = fopen_s(&fp, fileName, "wt")) != 0) {
		return;
	}

	//ヘッダ(インポート、Drawing生成)
	fputs("type,point1_x,point1_y,point2_x,point2_y,point3_x,point3_y\n", fp);

	for (auto connect : breadBoard.connections) {
		fputs(connect->type.toString().c_str(), fp);
		fputs(",", fp);
		fputs(to_string(connect->point1.x).c_str(), fp);
		fputs(",", fp);
		fputs(to_string(connect->point1.y).c_str(), fp);
		fputs(",", fp);
		fputs(to_string(connect->point2.x).c_str(), fp);
		fputs(",", fp);
		fputs(to_string(connect->point2.y).c_str(), fp);

		if (connect->type.type == PartType::TRAN_NPN || connect->type.type == PartType::TRAN_PNP) {
			ConnectionTransistor* trans = dynamic_cast<ConnectionTransistor*>(connect);
			fputs(",", fp);
			fputs(to_string(trans->point3.x).c_str(), fp);
			fputs(",", fp);
			fputs(to_string(trans->point3.y).c_str(), fp);
		}
		fputs("\n", fp);
	}

	fclose(fp);


	//コマンドから実行
	auto ret = system(nullptr);
	if (ret == 0) {
		MessageBox(NULL, TEXT("コマンドが実行できません"), MB_OK);
		return;
	}
	system("python Python/CircuitDiagram.py");
}

void CSampleDlg::drawParts(Mat& result) {

}

void CSampleDlg::createTestBoard() {
	//初期化
	//breadBoard.unusedHoles.clear();
	//breadBoard.usedHoles.clear();

	//for (int i = 0; i < 14; i++) {
	//	int col = 30;
	//	if (i <= 1 || 12 <= i)
	//		col = 25;
	//	
	//	for (int j = 0; j < col; j++) {
	//		breadBoard.unusedHoles.push_back(Point(i, j));
	//	}
	//}
}

bool CSampleDlg::isPartTypeTransistor(PartType type) {
	return type.type == PartType::TRAN_NPN || type.type == PartType::TRAN_PNP;
}

bool CSampleDlg::isPartTypeIC(PartType type) {
	return type.type == PartType::IC8 || type.type == PartType::IC14;
}

Point NO_INPUT = Point(-1, -1);

void CSampleDlg::drawLineConnect(Mat& result) {
	drawLineConnectInput(result, NO_INPUT);
}

void CSampleDlg::drawLineConnectInput(Mat& result, Point leftTop) {
	for (auto connect : breadBoard.connections) {
		Point hole1, hole2;
		if (isPartTypeTransistor(connect->type)) {
			ConnectionTransistor* trans = dynamic_cast<ConnectionTransistor*>(connect);
			hole1 = breadBoard.getHolePosition(trans->point1);
			hole2 = breadBoard.getHolePosition(trans->point3);
		}
		else {
			hole1 = breadBoard.getHolePosition(connect->point1);
			hole2 = breadBoard.getHolePosition(connect->point2);
		}

		if (leftTop != NO_INPUT) {
			if (connect->point1.y <= 1) {
				hole1.x += leftTop.x;
				hole1.y += leftTop.y;
			}
			else if (connect->point1.y <= 11) {
				hole1.x += leftTop.x;
				hole1.y += leftTop.y + LINE_HEIGHT;
			}
			else {
				hole1.x += leftTop.x;
				hole1.y += leftTop.y + LINE_HEIGHT + LINE_HEIGHT;
			}

			if (connect->point2.y <= 1) {
				hole2.x += leftTop.x;
				hole2.y += leftTop.y;
			}
			else if (connect->point2.y <= 11) {
				hole2.x += leftTop.x;
				hole2.y += leftTop.y + LINE_HEIGHT;
			}
			else {
				hole2.x += leftTop.x;
				hole2.y += leftTop.y + LINE_HEIGHT + LINE_HEIGHT;
			}
		}

		Scalar color;
		if (connect->type.type == PartType::WIRE) {
			color = GREEN;
		}
		else if (connect->type.type == PartType::RESISTOR) {
			color = YELLOW;
		}
		else if (connect->type.type == PartType::LED){
			color = BLUE;
		}
		else if (connect->type.type == PartType::CONDENSER) {
			color = PURPLE;
		}
		else if (connect->type.type == PartType::SWITCH) {
			color = LIGHTBLUE;
		}
		else if (isPartTypeTransistor(connect->type)) {
			color = ORANGE;
		}
		else if (isPartTypeIC(connect->type)) {
			color = GRAY;
			Point hole3 = Point(hole2.x, hole1.y);
			Point hole4 = Point(hole1.x, hole2.y);
			line(result, hole1, hole3, color, 3);
			line(result, hole1, hole4, color, 3);
			line(result, hole2, hole3, color, 3);
			line(result, hole2, hole4, color, 3);
			line(result, hole3, hole4, color, 3);
			circle(result, hole3, 10, GREEN, -1);
			circle(result, hole4, 10, GREEN, -1);
		}
		else {
			color = RED;
		}

		line(result, hole1, hole2, color, 3);
		circle(result, hole1, 10, GREEN, -1);
		circle(result, hole2, 10, GREEN, -1);
	}
}

void CSampleDlg::detectLineConnect() {
	//はじめにパーツ毎
	for (auto part : breadBoard.parts) {

		//２端子パーツ
		if(part->partType.type == PartType::WIRE
			|| part->partType.type == PartType::RESISTOR
			|| part->partType.type == PartType::LED
			|| part->partType.type == PartType::CONDENSER){
			vector<Point> edges;
			//端の穴をすべて取得
			for (auto position : part->holes) {
				HoleType holeType = breadBoard.getHoleType(position);
				if (holeType.type == HoleType::EDGE)
					edges.push_back(position);
			}
			if (edges.size() <= 1)
				continue;

			if (edges.size() == 2) {
				if(part->partType.type != PartType::LED)
					breadBoard.connections.push_back(new Connection(edges.at(0), edges.at(1), part->partType));
				else {
					//TODO:+-判定
					PartLED* led = dynamic_cast<PartLED*>(part);
					Connection* connect = new Connection(Point(0, 0), Point(0, 0), PartType::LED);
					if (judgeLEDAnode(led, edges.at(0), edges.at(1), connect)) {
						breadBoard.connections.push_back(connect);
					}
					else {
						//GUIでの入力
						breadBoard.connections.push_back(selectLEDAnode(led, edges.at(0), edges.at(1)));
					}
				}
			}
			else {
				//TODO:2端子でないエラーはいったん置いとく
				for (int i = 0; i < edges.size() - 1; i++) {
					for (int j = i + 1; j < edges.size(); j++) {
						breadBoard.connections.push_back(new Connection(edges.at(i), edges.at(j), part->partType));
					}
				}
			}
		}
		//3端子パーツ
		else if (isPartTypeTransistor(part->partType)) {
			PartTransistor* trans = dynamic_cast<PartTransistor*>(part);
			int top = INT_MAX;
			int under = 0;
			int left = INT_MAX;
			int right = 0;

			for (Point point : trans->holes) {
				top = min(top, point.y);
				under = max(under, point.y);
				left = min(left, point.x);
				right = max(right, point.x);
			}

			for (auto hole : trans->holes) {
				breadBoard.holeTypes.at(hole.y).at(hole.x) = HoleType::MIDDLE;
			}

			auto holes = trans->holes;

			if (trans->degree == 0) {
				breadBoard.holeTypes.at(under).at(left) = HoleType::EDGE;
				breadBoard.holeTypes.at(under).at(right) = HoleType::EDGE;
				breadBoard.holeTypes.at(under).at(left + 1) = HoleType::EDGE;

				Point leftUnder = Point(left, under);
				Point rightUnder = Point(right, under);
				Point centerUnder = Point(left+1, under);
				auto result = find(holes.begin(), holes.end(), leftUnder);
				if (result == holes.end())
					trans->addHole(leftUnder);
				result = find(holes.begin(), holes.end(), rightUnder);
				if (result == holes.end())
					trans->addHole(rightUnder);
				result = find(holes.begin(), holes.end(), centerUnder);
				if (result == holes.end())
					trans->addHole(centerUnder);

				breadBoard.connections.push_back(new  ConnectionTransistor(leftUnder, centerUnder, rightUnder, part->partType));
			}
			else {
				breadBoard.holeTypes.at(top).at(left) = HoleType::EDGE;
				breadBoard.holeTypes.at(top).at(right) = HoleType::EDGE;
				breadBoard.holeTypes.at(top).at(left + 1) = HoleType::EDGE;

				Point leftTop = Point(left, top);
				Point  rightTop = Point(right, top);
				Point centerTop = Point(left + 1, top);
				auto result = find(holes.begin(), holes.end(), leftTop);
				if (result == holes.end())
					trans->addHole(leftTop);
				result = find(holes.begin(), holes.end(), rightTop);
				if (result == holes.end())
					trans->addHole(rightTop);
				result = find(holes.begin(), holes.end(), centerTop);
				if (result == holes.end())
					trans->addHole(centerTop);

				breadBoard.connections.push_back(new  ConnectionTransistor(leftTop, centerTop, rightTop, part->partType));
			}
		}
		//4端子パーツ
		else if (part->partType.type == PartType::SWITCH) {
			PartSwitch* swt = dynamic_cast<PartSwitch*>(part);
			int top = INT_MAX;
			int under = 0;
			int left = INT_MAX;
			int right = 0;

			for (Point point : swt->holes) {
				top = min(top, point.y);
				under = max(under, point.y);
				left = min(left, point.x);
				right = max(right, point.x);
			}
			Point leftTop = Point(left, top);
			Point leftUnder = Point(left, under);
			Point rightTop = Point(right, top);
			Point rightUnder = Point(right, under);
			breadBoard.holeTypes.at(top).at(left) = HoleType::EDGE;
			breadBoard.holeTypes.at(top).at(right) = HoleType::EDGE;
			breadBoard.holeTypes.at(under).at(left) = HoleType::EDGE;
			breadBoard.holeTypes.at(under).at(right) = HoleType::EDGE;
			auto holes = swt->holes;
			auto result = find(holes.begin(), holes.end(), leftTop);
			if (result == holes.end())
				swt->addHole(leftTop);
			result = find(holes.begin(), holes.end(), leftUnder);
			if (result == holes.end())
				swt->addHole(leftUnder);
			result = find(holes.begin(), holes.end(), rightTop);
			if (result == holes.end())
				swt->addHole(rightTop);
			result = find(holes.begin(), holes.end(), rightUnder);
			if (result == holes.end())
				swt->addHole(rightUnder);

			if ((int)(swt->degree) % 180 == 0) {
				breadBoard.connections.push_back(new Connection(leftTop, rightTop, PartType::SWITCH));
				breadBoard.connections.push_back(new Connection(leftUnder, rightUnder, PartType::SWITCH));
				breadBoard.connections.push_back(new Connection(leftTop, leftUnder, PartType::WIRE));
				breadBoard.connections.push_back(new Connection(rightTop, rightUnder, PartType::WIRE));
			}
			else {
				breadBoard.connections.push_back(new Connection(leftTop, leftUnder, PartType::SWITCH));
				breadBoard.connections.push_back(new Connection(rightTop, rightUnder, PartType::SWITCH));
				breadBoard.connections.push_back(new Connection(leftTop, rightTop, PartType::WIRE));
				breadBoard.connections.push_back(new Connection(leftUnder, rightUnder, PartType::WIRE));
			}
		}
		//ic
		else if (isPartTypeIC(part->partType)) {
			int top = INT_MAX;
			int under = 0;
			int left = INT_MAX;
			int right = 0;

			for (Point point : part->holes) {
				top = min(top, point.y);
				under = max(under, point.y);
				left = min(left, point.x);
				right = max(right, point.x);
			}
			if (part->partType.type == PartType::IC14) {
				left++;
				right--;
			}
			Point leftTop = Point(left, top);
			Point leftUnder = Point(left, under);
			Point rightTop = Point(right, top);
			Point rightUnder = Point(right, under);
			for (int p = left; p <= right; p++) {
				breadBoard.holeTypes.at(top).at(p) = HoleType::EDGE;
				breadBoard.holeTypes.at(under).at(p) = HoleType::EDGE;
			}
			auto holes = part->holes;
			auto result = find(holes.begin(), holes.end(), leftTop);
			if (result == holes.end())
				part->addHole(leftTop);
			result = find(holes.begin(), holes.end(), leftUnder);
			if (result == holes.end())
				part->addHole(leftUnder);
			result = find(holes.begin(), holes.end(), rightTop);
			if (result == holes.end())
				part->addHole(rightTop);
			result = find(holes.begin(), holes.end(), rightUnder);
			if (result == holes.end())
				part->addHole(rightUnder);

			breadBoard.connections.push_back(new Connection(leftTop, rightUnder, part->partType));
		}
	}

	//最後に縦列
	for (auto position : breadBoard.usedHoles) {
		Point hole = breadBoard.holePositions.at(position.y).at(position.x);
		HoleType holeType = breadBoard.holeTypes.at(position.y).at(position.x);

		//+-エリアなら無視
		if (position.y < 2 || 11 < position.y)
			continue;

		//端のみつなげる
		if (holeType.type == HoleType::EDGE) {
			for (int i = 1; i < 5; i++) {
				int nextY = position.y + i;
				//分断部分で終わる
				if (nextY == 7 || nextY == 12)
					break;

				HoleType nextHoleType = breadBoard.holeTypes.at(nextY).at(position.x);
				if (nextHoleType.type == HoleType::EDGE) {
					Point nextHole = breadBoard.holePositions.at(nextY).at(position.x);
					Point nextPosition = Point(position.x, position.y + i);
					breadBoard.connections.push_back(new Connection(position, nextPosition, PartType::WIRE));
					break;
				}
			}
		}
	}
}

/// <summary>
/// LEDのアノード側を判定する
/// </summary>
/// <param name="led">LEDパーツオブジェクト</param>
/// <param name="p1">端の穴の一つ目</param>
/// <param name="p2">端の穴の２つ目</param>
/// <param name="connect">判定結果のConnectionオブジェクト</param>
/// <returns>
///		判定不可能：false
///		判定可能：true　(connectの中身が判定結果に更新)
/// </returns>
boolean CSampleDlg::judgeLEDAnode(PartLED* led, Point p1, Point p2, Connection* connect) {
	Point point1 = breadBoard.holePositions.at(p1.y).at(p1.x);
	Point point2 = breadBoard.holePositions.at(p2.y).at(p2.x);

	Point global = Point(led->position.x, led->position.y);
	Point headG = led->headPosition + global;
	Point headCenter = Point(led->head.cols / 2, led->head.rows / 2) + headG;

	float p1Dis = norm(point1 - headCenter);
	float p2Dis = norm(point2 - headCenter);
	float borderCanJudge = 0;
	
	//imshow("input", led->mat);
	//imshow("head", led->head);


	if (p1Dis > p2Dis && p1Dis - p2Dis >= borderCanJudge) {
		connect = new Connection(p1, p2, PartType::LED);
		return true;
	}
	else if (p2Dis > p1Dis && p2Dis - p1Dis >= borderCanJudge) {
		connect = new Connection(p2, p1, PartType::LED);
		return true;
	}

	return false;
}

/// <summary>
/// LEDのアノード側をGUI操作でユーザーに選択させる
/// </summary>
/// <param name="led">LEDパーツオブジェクト</param>
/// <param name="p1">端の穴の一つ目</param>
/// <param name="p2">端の穴の２つ目</param>
/// <returns>
///		アノード側をpoint1、カソード側をpoint2にしたConnectionオブジェクト
/// </returns>
Connection* CSampleDlg::selectLEDAnode(PartLED* led, Point p1, Point p2) {
	String windowName = "select anode(+)";
	//String windowAll = "all";

	//PartLED* led = dynamic_cast<PartLED*>(part);
	mouseParam mouseEvent;
	Point global = Point(led->position.x, led->position.y);
	Point point1G = breadBoard.holePositions.at(p1.y).at(p1.x); //入力画像での位置
	Point point1P = point1G - global;//パーツ画像での位置
	Point point2G = breadBoard.holePositions.at(p2.y).at(p2.x);
	Point point2P = point2G - global;


	//全体画像生成
	//むしろわかりにくい
	//Mat all = concatInput.clone();
	//rectangle(all, part->position, YELLOW, 2);
	//imshow(windowAll, all);
	//表示画像生成
	Mat selectShow = led->mat.clone();
	int rectSize = 10;
	Rect area1 = Rect(Point(point1P.x - rectSize, point1P.y - rectSize), Point(point1P.x + rectSize, point1P.y + rectSize));
	Rect area2 = Rect(Point(point2P.x - rectSize, point2P.y - rectSize), Point(point2P.x + rectSize, point2P.y + rectSize));
	rectangle(selectShow, area1, YELLOW, 2);
	rectangle(selectShow, area2, YELLOW, 2);
	imshow(windowName, selectShow);
	setMouseCallback(windowName, callBackMouseFunc, &mouseEvent);

	//マウス操作
	while (1) {
		cv::waitKey(20);
		if (mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
			destroyWindow(windowName);
			//destroyWindow(windowAll);

			Point mouseClick = Point(mouseEvent.x, mouseEvent.y) + global;
			float p1Distance = norm(point1G - mouseClick);
			float p2Distance = norm(point2G - mouseClick);

			if (p1Distance < p2Distance)
				return new Connection(p1, p2, PartType::LED);
			else
				return new Connection(p2, p1, PartType::LED);
		}
	}
}

void CSampleDlg::cutParts(Mat input, Mat& result, Mat mask, Mat labels, Mat status) {
	String partsPath = RESULT_PATH + "parts\\";
	
	set<int> saveLabel;
	map<int, int>labelIndex;
	int index = 0;
	for (auto itr = breadBoard.usedHoles.begin(); itr != breadBoard.usedHoles.end(); ++itr) {
		Point position = *itr;
		Point usedHole = breadBoard.holePositions.at(position.y).at(position.x);

		int* label = labels.ptr<int>(usedHole.y, usedHole.x);
		if (*label == 0) {
			//背景画素のはずがないので、周りを探す。
			int len = 10; //周り10の長さまで探す。
			for (int l = 1; l <= len; l++) {
				for (int i = 0; i < l; i++) {
					//上下
					if (i == 0 || i == len) {
						for (int j = 0; j < l; j++) {
							int* lab = labels.ptr<int>(usedHole.y + i - (len / 2), usedHole.x + j - (len / 2));
							if (*lab != 0) {
								label = lab;
								goto SEARCHED;
							}
						}
					}
					//間
					else {
						int* lab = labels.ptr<int>(usedHole.y + i - (len / 2), usedHole.x - (len / 2));
						if (*lab != 0) {
							label = lab;
							goto SEARCHED;
						}
						lab = labels.ptr<int>(usedHole.y + i - (len / 2), usedHole.x + (len / 2));
						if (*lab != 0) {
							label = lab;
							goto SEARCHED;
						}
					}
				}
			}

			continue;
		}

		SEARCHED:

		decltype(saveLabel)::iterator it = saveLabel.find(*label);
		if (it == saveLabel.end()) {
			saveLabel.insert(*label);
			int partSize = 0;

			int* param = status.ptr<int>(*label);
			int top = param[ConnectedComponentsTypes::CC_STAT_TOP];
			int left = param[ConnectedComponentsTypes::CC_STAT_LEFT];
			int height = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
			int down = top + height;
			int right = left + width;
			Rect area = Rect(left, top, width, height);

			Mat partRaw = Mat(input, area).clone();
			Mat partMask = Mat(mask, area).clone();
			Mat partPosition = input.clone();
			Mat partCut;
			partRaw.copyTo(partCut, partMask);
			Mat partHSV;
			cvtColor(partRaw, partHSV, CV_RGB2HSV);

			//入力画像にパーツの場所を示す
			rectangle(partPosition, area, Scalar(0, 0, 255), 2);

			//ラベルで切り抜き
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					int* ptr = partCut.ptr<int>(i, j);
					if (*ptr == 0)
						continue;

					int y = top + i;
					int x = left + j;
					int* l = labels.ptr<int>(y, x);
					if (*l != *label) {
						*ptr = 0;
						*(ptr + 1) = 0;
						*(ptr + 2) = 0;
					}
					else
						partSize++;

				}
			}

			//パーツ種類判定
			////パーツ登録
			Part* part = judgePartType(partCut, partSize, area);
			PartType type = part->getPartType();
			part->addHole(position);
			breadBoard.parts.push_back(part);
			labelIndex[*label] = index;
			index++;

			//画像保存
			imwrite(partsPath + format("%d_", *label) + type.toString() + "_raw.bmp", partRaw);
			imwrite(partsPath + format("%d_", *label) + type.toString() + "_mask.bmp", partMask);
			imwrite(partsPath + format("%d_", *label) + type.toString() + "_position.bmp", partPosition);
			imwrite(partsPath + format("%d_", *label) + type.toString() + "_cut.bmp", partCut);
			imwrite(partsPath + format("%d_", *label) + type.toString() + "_hsv.bmp", partHSV);
			imwrite(partsPath + "position_" + format("%d_", *label) + "_" + type.toString() + ".bmp", partPosition);
		}
		//すでにPartsを登録済み
		else {
			int partIndex = labelIndex[*label];
			breadBoard.parts.at(partIndex)->addHole(position);
		}

	}
}

Part* CSampleDlg::judgePartType(Mat input, int size, Rect area) {
	//PartType type;
	Mat hsv;
	cvtColor(input, hsv, CV_RGB2HSV);
	int debug = 0;

	//単色判定(銅線)
	float singleColorRation = 0.8;
	int singleColorRange = 20;
	int singleColorDistance = 10;
	int singleColorDivision = 360 / singleColorDistance;
	for (int r = 0; r < singleColorDivision; r++) {
		int rangeBase = r * singleColorDistance;
		Scalar hmin = Scalar(rangeBase, 0, 50);
		Scalar hmax = Scalar(rangeBase + singleColorRange, 255, 255);
		Mat mask;
		inRange(hsv, hmin, hmax, mask);

		int count = 0;
		for (int i = 0; i < input.rows; i++) {
			for (int j = 0; j < input.cols; j++) {
				int* inputPtr = input.ptr<int>(i, j);
				if (*inputPtr != 0) {
					int* maskPtr = mask.ptr<int>(i, j);
					if (*maskPtr != 0)
						count++;
				}
			}
		}

		if (count >= size * singleColorRation) {
			return new Part(input.clone(), area, size, PartType::WIRE);
		}
	}

	//テンプレートマッチング////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Mat input_dst;
	int extSize = 90; //拡張するサイズ
	copyMakeBorder(input, input_dst, extSize, extSize, extSize, extSize, BORDER_CONSTANT, Scalar(0, 0, 0));

	Scalar mask_min(1, 1, 1);
	Scalar mask_max(255, 255, 255);
	Mat input_mask;
	inRange(input_dst, mask_min, mask_max, input_mask);
	morphologyEx(input_mask, input_mask, MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(5, 5)));
	cvtColor(input_mask, input_mask, CV_GRAY2BGR);

	vector<String> partNames{ "resistor", "LED", "condenser", "switch", "transistor", "ic8", "ic16" };
	int partTypeSize = partNames.size();
	map<String, Mat> partTemp;
	map<String, Point> partMaxPt;
	map<String, double> partDeg;
	map<String, double> partMaxVal;
	map<String, int> partSize;
	map<String, Point2f> partCenter;
	map<String, Mat> partMove;
	Mat temp_result;

	for (String name : partNames) {
		String pathName = "./res/template/" + name + "_mask.bmp";
		Mat part = imread(pathName);
		int size = max(part.rows, part.cols);
		Point2f center = Point2f((size / 2), (size / 2));
		Mat move = (Mat_<double>(2, 3) << 1, 0, center.x - part.cols / 2,
			0, 1, center.y - part.rows / 2);
		partTemp[name] = part;
		partSize[name] = size;
		partCenter[name] = center;
		partMove[name] = move;
		partMaxVal[name] = 0;
	}

	String imageName = format("%d,%d", area.x, area.y);

	//画像の拡張に対して、areaを変更
	area.x -= extSize;
	area.y -= extSize;
	area.width += extSize * 2;
	area.height += extSize * 2;

	//テンプレ画像の回転
	/* TODO:なぜ1になってる？*/
	for (int i = 0; i < 1; i++) {
		double degree = (double)i * 10;
		map<String, Mat> partRotate;
		map<String, Mat> partAffine;
		int method = CV_TM_CCOEFF_NORMED;

		for (String name : partNames) {
			Point partPt;
			double partVal;
			Mat rotate = getRotationMatrix2D(partCenter[name], degree, 1.0);
			partRotate[name] = rotate;
			Mat affine; 
			//平行移動
			warpAffine(partTemp[name], affine, partMove[name], Size(partSize[name], partSize[name]), INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));
			//回転
			warpAffine(affine, affine, partRotate[name], Size(partSize[name], partSize[name]), INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));

			//テンプレートマッチング
			if (input_mask.rows >= affine.rows
				&& input_mask.cols >= affine.cols) {
				matchTemplate(input_mask, affine, temp_result, method);
				minMaxLoc(temp_result, NULL, &partVal, NULL, &partPt);
				if (partVal > partMaxVal[name]) {
					partMaxVal[name] = partVal;
					partMaxPt[name] = partPt;
					partDeg[name] = degree;
				}
			}
		}
	}

	double maxVal = 0;
	String matchName;
	for (String name : partNames) {
		if (maxVal < partMaxVal[name]) {
			maxVal = partMaxVal[name];
			matchName = name;
		}
	}

	double border = 0.45;
	if (maxVal < border)
		return new Part(input.clone(), area, size, PartType::WIRE);

	//resistor
	if (matchName == partNames[0]) {
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		return new Part(input.clone(), area, size, PartType::RESISTOR);
	}
	//LED
	else if (matchName == partNames[1]) {
		//頭頂部の端穴を削除
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input_dst.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		removeLEDTop(roi, area);
		Mat head = Mat(input_dst, roi);
		Point headPosition = Point(roi.x, roi.y);
		return new PartLED(input.clone(), area, size, head, headPosition);
	}
	//condenser
	else if (matchName == partNames[2]) {
		//頭頂部の端穴を削除
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input_dst.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		removeLEDTop(roi, area);
		Mat head = Mat(input_dst, roi);
		Point headPosition = Point(roi.x, roi.y);
		return new PartCondenser(input.clone(), area, size, head, headPosition);
	}
	// switch
	else if (matchName == partNames[3]) {
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		return new PartSwitch(input.clone(), area, size, partDeg[matchName]);
	}
	//transistor
	else if (matchName == partNames[4]) {
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		PartType type = selectTransistorType(input);
		return new PartTransistor(input.clone(), area, size, partDeg[matchName], type);
	}
	//ic8
	else if (matchName == partNames[5]) {
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		return new Part(input.clone(), area, size, PartType::IC8);
	}
	//ic16
	else if (matchName == partNames[6]) {
		Rect roi(partMaxPt[matchName].x, partMaxPt[matchName].y, partTemp[matchName].cols, partTemp[matchName].rows);
		Mat templete = input.clone();
		rectangle(templete, roi, RED, 2);
		saveImageTemp(templete, imageName);
		return new Part(input.clone(), area, size, PartType::IC14);
	}


	//エラー用のもの作る？
	return new Part(input.clone(), area, size, PartType::WIRE);
}

PartType CSampleDlg::selectTransistorType(Mat part) {
	String windowName = "select anode(+)";

	//表示画像生成
	namedWindow(windowName, WINDOW_NORMAL);
	imshow(windowName, part);

	int result = MessageBox(TEXT("トランジスタのタイプはPNPですか？"), TEXT("トランジスタの確認"), MB_YESNO);
	destroyWindow(windowName);
	
	if (result == IDYES) return PartType::TRAN_PNP;
	else return PartType::TRAN_NPN;
}

void CSampleDlg::removeLEDTop(Rect roi, Rect partArea) {
	Rect area = roi;
	area.x += partArea.x;
	area.y += partArea.y;
	int areaExt = 20;

	for (int i = 0; i < 14; i++) {
		int row = 30;
		if (i < 2 || 11 < i)
			row = 25;
		for (int j = 0; j < row; j++) {
			HoleType type = breadBoard.holeTypes.at(i).at(j);
			
			if (type.type == HoleType::EDGE) {
				Point hole = breadBoard.holePositions.at(i).at(j);
				if (area.x - areaExt <= hole.x && hole.x <= area.x + area.width + areaExt
					&& area.y - areaExt <= hole.y && hole.y <= area.y + area.height + areaExt) {

					breadBoard.holeTypes.at(i).at(j) = HoleType::MIDDLE;
				}

			}
		}
	}
}

/// <summary>
/// 穴が使用しているかを検出
/// </summary>
/// <param name="hole">穴の位置。使用している場合は位置を中央に更新する</param>
/// <param name="labels">ラベル情報</param>
/// <param name="status">ラベリングのステータス情報</param>
/// <returns>
///		使用:true
///		未使用:false
/// </returns>
bool CSampleDlg::checkHoleUsed(Point& hole, Mat& result, Mat labels, Mat status) {
	if (hole.x >= labels.cols || hole.y >= labels.rows)
		return false;

	//穴があったら表示　+　補正
	int* label = labels.ptr<int>(hole.y, hole.x);
	if (*label != 0) {
		int* param = status.ptr<int>(*label);

		int x = param[ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[ConnectedComponentsTypes::CC_STAT_TOP];
		int w = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
		int h = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
		hole = Point(x + w / 2, y + h / 2);

		rectangle(result, Rect(Point(x, y), Point(x + w, y + h)), Scalar(0, 255, 0), 2);
		return false;
	}

	int top = hole.y - 1;
	if (top < 0) top = 0;
	int down = hole.y + 1;
	if (down >= result.rows) down = result.rows - 1;
	int left = hole.x - 1;
	if (left < 0) left = 0;
	int right = hole.x + 1;
	if (right >= result.cols) right = result.cols - 1;

	//左
	label = labels.ptr<int>(hole.y, left);
	if (*label != 0) {
		int* param = status.ptr<int>(*label);
		int x = param[ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[ConnectedComponentsTypes::CC_STAT_TOP];
		int w = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
		int h = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
		hole = Point(x + w / 2, y + h / 2);

		rectangle(result, Rect(Point(x, y), Point(x + w, y + h)), Scalar(0, 255, 0), 2);
		return false;
	}

	//上
	label = labels.ptr<int>(top, hole.x);
	if (*label != 0) {
		int* param = status.ptr<int>(*label);
		int x = param[ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[ConnectedComponentsTypes::CC_STAT_TOP];
		int w = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
		int h = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
		hole = Point(x + w / 2, y + h / 2);

		rectangle(result, Rect(Point(x, y), Point(x + w, y + h)), Scalar(0, 255, 0), 2);
		return false;
	}

	//右
	label = labels.ptr<int>(hole.y, right);
	if (*label != 0) {
		int* param = status.ptr<int>(*label);
		int x = param[ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[ConnectedComponentsTypes::CC_STAT_TOP];
		int w = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
		int h = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
		hole = Point(x + w / 2, y + h / 2);

		rectangle(result, Rect(Point(x, y), Point(x + w, y + h)), Scalar(0, 255, 0), 2);
		return false;
	}

	//下
	label = labels.ptr<int>(down, hole.x);
	if (*label != 0) {
		int* param = status.ptr<int>(*label);
		int x = param[ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[ConnectedComponentsTypes::CC_STAT_TOP];
		int w = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
		int h = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
		hole = Point(x + w / 2, y + h / 2);

		rectangle(result, Rect(Point(x, y), Point(x + w, y + h)), Scalar(0, 255, 0), 2);
		return false;
	}

	return true;

}

void CSampleDlg::detectBoardHole(Mat input, Mat& result, Point leftTop, Mat labels, Mat status) {
	result = input.clone();
	int lineHeigth = 50;
	Point DIS_BOARD_TO_HOLE(120, 67);
	Point DIS_NEXT_HOLE(45, 45);
	int DIS_NEXT_LONG_X = 90;
	Point DIS_PLUS_TO_J(-16, 124 - lineHeigth);

	//暫定処理
	Point DIS_A_TO_MINUS(25, 130 - lineHeigth);
	//Point DIS_A_TO_MINUS(25, 110 - lineHeigth);
	int DIS_F_TO_E = 130;
	//int DIS_F_TO_E = 80;

	Point hole(leftTop.x + DIS_BOARD_TO_HOLE.x, leftTop.y + DIS_BOARD_TO_HOLE.y);
	Point leftHole = hole;
	int i = 0;
	int j = 0;

	for (i = 0; i < 14; i++) {
		std::vector<Point> positions;
		std::vector<HoleType> holeTypes;

		//+-列の穴
		if (i == 0 || i == 1 || i == 12 || i == 13) {
			for (j = 0; j < 25; j++) {

				//穴があったら表示　+　補正
				if (checkHoleUsed(hole, result, labels, status)) {
					breadBoard.usedHoles.push_back(Point(j, i));
					holeTypes.push_back(HoleType::MIDDLE);
				}
				else {
					breadBoard.unusedHoles.push_back(Point(j, i));
					holeTypes.push_back(HoleType::EMPTY);
				}

				positions.push_back(hole);

				if (j == 0) {
					leftHole = hole;
				}

				if (j % 5 == 4) 
					hole.x += DIS_NEXT_LONG_X;
				else
					hole.x += DIS_NEXT_HOLE.x;
			}
		}
		else {
			for (j = 0; j < 30; j++) {

				//穴があったら表示　+　補正
				if (checkHoleUsed(hole, result, labels, status)) {
					breadBoard.usedHoles.push_back(Point(j, i));
					holeTypes.push_back(HoleType::MIDDLE);
				}
				else {
					breadBoard.unusedHoles.push_back(Point(j, i));
					holeTypes.push_back(HoleType::EMPTY);
				}

				positions.push_back(hole);

				if (j == 0) {
					leftHole = hole;
				}

				hole.x += DIS_NEXT_HOLE.x;
			}
		}

		hole = leftHole;
		if (i == 1) {
			hole.x += DIS_PLUS_TO_J.x;
			hole.y += DIS_PLUS_TO_J.y;
		}
		else if (i == 11) {
			hole.x += DIS_A_TO_MINUS.x;
			hole.y += DIS_A_TO_MINUS.y;
		}
		else if (i == 6) {
			hole.y += DIS_F_TO_E;
		}
		else {
			hole.y += DIS_NEXT_HOLE.y;
		}

		breadBoard.holePositions.push_back(positions);
		breadBoard.holeTypes.push_back(holeTypes);
	}

	//再判定
	reCheckHoleUsed(result, labels, status);
}

void CSampleDlg::reCheckHoleUsed(Mat& result, Mat labels, Mat status) {
	Point DISTANCE(30 * 1.5, 30 * 1.5);

	vector<Point> reCheck(breadBoard.usedHoles.size());
	copy(breadBoard.usedHoles.begin(), breadBoard.usedHoles.end(), reCheck.begin());
	vector<Point> reCheckChain;

	while (!reCheck.empty()) {
		auto itr = reCheck.begin();
		Point position = *itr;
		reCheck.erase(itr);
		if (!breadBoard.checkUsed(position))
			continue;

		Point hole = breadBoard.holePositions.at(position.y).at(position.x);
		list<int> colList;
		int sum = 0;
		int count = 0;

		//+-線
		if (position.y <= 1 || 12 <= position.y) {
			colList.push_back(0);
			colList.push_back(1);
			colList.push_back(12);
			colList.push_back(13);
		}
		else {
			for (int i = 2; i <= 11; i++) {
				colList.push_back(i);
			}
		}

		//同列の未使用穴の座標を使用する
		for (int y : colList) {
			if (!breadBoard.checkUsed(y, position.x)) {
				Point p = breadBoard.holePositions.at(y).at(position.x);
				sum += p.x;
				count++;
			}
		}

		//
		if (count == 0)
			continue;

		int ave = sum / count;
		hole.x = ave;
		//判定関係なく位置は更新する
		breadBoard.holePositions.at(position.y).at(position.x) = hole;

		//再判定
		if (!checkHoleUsed(hole, result, labels, status)) {
			breadBoard.unusedHoles.push_back(position);
			breadBoard.deleteUsedHole(position);
			breadBoard.holeTypes.at(position.y).at(position.x) = HoleType::EMPTY;

			if (breadBoard.checkUsed(position.y, position.x + 1)) 
				reCheckChain.push_back(Point(position.x + 1, position.y));
			//TODO:
			//縦の連鎖はいったん無くす
			//if (position.x == 0 && breadBoard.checkUsed(position.y + 1, position.x))
			//	reCheckChain.push_back(Point(position.y + 1, position.x));

			//連鎖判定
			while (!reCheckChain.empty()) {
				auto itr2 = reCheckChain.begin();
				Point nextHole = *itr2;
				reCheckChain.erase(itr2);

				int y = nextHole.y;
				int x = (int)nextHole.x - 1;
				Point baseHole = breadBoard.holePositions.at(y).at(x);
				Point p = Point(baseHole.x + DISTANCE.x, baseHole.y);

				if (!checkHoleUsed(p, result, labels, status)) {
					breadBoard.unusedHoles.push_back(nextHole);
					breadBoard.deleteUsedHole(nextHole);
					breadBoard.holeTypes.at(position.y).at(position.x) = HoleType::EMPTY;
				}

				breadBoard.holePositions.at(nextHole.y).at(nextHole.x) = p;
				if (breadBoard.checkUsed(nextHole.y, nextHole.x + 1)) {
					reCheckChain.push_back(Point(nextHole.x + 1, nextHole.y));
				}
			}
		}
	}
}

void CSampleDlg::judgeHoleType(Mat input, Mat& result) {
	result = input.clone();


	vector<Point> used(breadBoard.usedHoles.size());
	copy(breadBoard.usedHoles.begin(), breadBoard.usedHoles.end(), used.begin());

	//使用している穴
	while (!used.empty()) {
		auto itr = used.begin();
		Point position = *itr;
		used.erase(itr);

		Point usedHole = breadBoard.holePositions.at(position.y).at(position.x);
		int s = 10 * 1.5;
		int left = usedHole.x - s;
		int top = usedHole.y - s;
		int right = usedHole.x + s;
		int down = usedHole.y + s;

		HoleType holeType = saveHole(position);
		Scalar color;
		if (holeType.type == HoleType::MIDDLE)
			color = Scalar(255, 255, 0);
		else if (holeType.type == HoleType::EDGE)
			color = Scalar(0, 255, 255);
		else if (holeType.type == HoleType::EMPTY) {
			color = Scalar(0, 255, 0);
			breadBoard.deleteUsedHole(position);
			breadBoard.unusedHoles.push_back(position);
		}

		breadBoard.holeTypes.at(position.y).at(position.x) = holeType;


		rectangle(result, Rect(left, top, s * 2, s * 2), color, 2);
	}
}

HoleType CSampleDlg::saveHole(Point position) {
	int d = 0;
	if (position.y == 1 && position.x == 7)
		d++;

	Point usedHole = breadBoard.holePositions.at(position.y).at(position.x);
	int size = 25 * 1.5;
	int x = usedHole.x - size;
	if (x < 0) return HoleType::EDGE;
	int y = usedHole.y - size;
	if (y < 0) return HoleType::EDGE;
	int w = usedHole.x + size;
	if (w >= concatInput.cols) return HoleType::EDGE;
	int h = usedHole.y + size;
	if (h >= concatInput.rows) return HoleType::EDGE;

	Mat holeRaw = Mat(concatInput, Rect(Point(x, y), Point(w, h))).clone();
	String path = RESULT_PATH + "holes\\";
	String holeName =  "hole_" + BreadBoard::getHoleName(position);
	HoleType holeType;

	Mat hsv, gray;
	cvtColor(holeRaw, gray, CV_BGR2GRAY);
	cvtColor(holeRaw, hsv, CV_BGR2HSV);
	
	unsigned char* color = hsv.ptr<unsigned char>(size, size);
	unsigned char hsvValue[3] = { *(color + 2) , *(color + 1), *color };
	unsigned char* kido = gray.ptr<unsigned char>(size, size);

	//if (hsvValue[1] > 150)
	//	holeType.type = HoleType::MIDDLE;
	//else if (*kido < 50)
	//	holeType.type = HoleType::MIDDLE;
	//else if (*kido > 175)
	//	holeType.type = HoleType::MIDDLE;
	if (false)
		printf("");
	//端と判定されたらさらにラベリングで判定
	else {

		Mat range, range_rev;
		Scalar sMin = Scalar(0, 0, 130);
		Scalar sMax = Scalar(180, 20, 255);
		inRange(hsv, sMin, sMax, range);
		cv::bitwise_not(range, range_rev);

		//膨張
		morphologyEx(range_rev, range_rev, MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(5, 5)));

		Mat labels, status, centroids;
		connectedComponentsWithStats(range_rev, labels, status, centroids, 8, CV_32S);
		int* label = labels.ptr<int>(size, size);
		Mat cutLabel;
		Scalar u = Scalar(*label, *label, *label);
		inRange(labels, u, u, cutLabel);

		int* param = status.ptr<int>(*label);
		int left = param[ConnectedComponentsTypes::CC_STAT_LEFT];
		int top = param[ConnectedComponentsTypes::CC_STAT_TOP];
		int width = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
		int height = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
		int right = left + width;
		int down = top + height;

		if (width >= size * 2 || height >= size * 2)
			holeType = HoleType::MIDDLE;
		else {
			int edgeCount = 0;
			if (left == 0)
				edgeCount++;
			if (right == size * 2)
				edgeCount++;
			if (top == 0)
				edgeCount++;
			if (down == size * 2)
				edgeCount++;

			if (edgeCount >= 2) {
				vector<vector<Point>> contours;
				vector<Vec4i> hierarchy;
				findContours(cutLabel, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

				int edgeCount = 0;
				bool currentEdge = false;
				bool prevEdge = currentEdge;
				bool startCount = true;
				for (auto point : contours.at(0)) {
					if (point.x == 0 || point.x == size * 2 - 1
						|| point.y == 0 || point.y == size * 2 - 1)
						currentEdge = true;
					else
						currentEdge = false;

					if (startCount) 
						startCount = false;	
					else if (currentEdge != prevEdge)
						edgeCount++;

					prevEdge = currentEdge;
				}

				if(edgeCount == 2)
					holeType.type = HoleType::EDGE;
				else
					holeType.type = HoleType::MIDDLE;
			}
			else if (edgeCount == 0) { //どの端にも達していないなら搭載なしと再判定
				holeType.type = HoleType::EMPTY;
				//TODO: holeをusedからunusedに移す
			}
			else
			holeType.type = HoleType::EDGE;
		}

		//imwrite(path + holeName + "_range" + ".bmp", range);
		imwrite(path + holeName + "_range_rev" + ".bmp", range_rev);
		//imwrite(path + holeName + "_labels" + ".bmp", labels);
		imwrite(path + holeName + "_labels" + ".bmp", cutLabel);
	}

	//rectangle(holeRaw, Rect(Point(size - 2, size-2), Point(size+2, size+2)), Scalar(0, 0, 255));
	imwrite(path + holeName + "_raw" + ".bmp", holeRaw);

	return holeType;
}

void CSampleDlg::getBoardRect(const Mat input, Rect& area) {
	//HSV
	Mat hsv;
	cvtColor(input, hsv, CV_BGR2HSV);
	Scalar sMin = Scalar(0, 0, 150);
	Scalar sMax = Scalar(180, 100, 255);
	Mat mask;
	inRange(hsv, sMin, sMax, mask);

	//クロージング処理
	morphologyEx(mask, mask, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(10, 10)));

	//メディアンフィルタ
	Mat median;
	medianBlur(mask, median, 9);

	//ラベリング
	Mat labels, status, centroids;
	connectedComponentsWithStats(median, labels, status, centroids, 8, CV_32S);
	int maxSize = 0;
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			int* label = labels.ptr<int>(i, j);
			int* param = status.ptr<int>(*label);
			int size = param[ConnectedComponentsTypes::CC_STAT_AREA];
			if (size > maxSize) {
				maxSize = size;
				area.x = param[ConnectedComponentsTypes::CC_STAT_LEFT];
				area.y = param[ConnectedComponentsTypes::CC_STAT_TOP];
				area.width = param[ConnectedComponentsTypes::CC_STAT_WIDTH];
				area.height = param[ConnectedComponentsTypes::CC_STAT_HEIGHT];
			}
		}
	}

	//矩形探索
	//area = boundingRect(median);
}

void CSampleDlg::OnHoukoku()
{
	Mat input = imread((String)RESULT_PATH + "test.bmp");
	imwrite((String)RESULT_PATH + "test.png", input);

	Mat hsv;
	cvtColor(input, hsv, CV_BGR2HSV);
	Scalar sMin = Scalar(0, 0, 130);
	Scalar sMax = Scalar(180, 20, 255);
	Mat mask;
	inRange(hsv, sMin, sMax, mask);

	imshow("mask", mask);
	imwrite((String)RESULT_PATH + "mask.bmp", mask);
}

void CSampleDlg::drawOnLine(OnLine line) {
	//for (Connection chain : line.getChains()) {
	//	m_Image->drawLine(chain.point1, chain.point2);
	//}
}

void CSampleDlg::UpdateImage(int height, int width) {
	//表示用
	for (int i = 0; i < height; i++) {
		for (int j = 0; j <width; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j] = m_ChangeImage->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j] = m_ChangeImage->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j] = m_ChangeImage->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

void CSampleDlg::UpdateImage() {
	//表示用
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j] = m_ChangeImage->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j] = m_ChangeImage->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j] = m_ChangeImage->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//半分ブラックボックス
Image* CSampleDlg::OpenImage(CString fileName) {
	int i, j;
	size_t image_size;
	//CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;
	unsigned char* bmpImage;
	LPBITMAPINFO bmpInfo;
	Image* image = new Image(0, 0);
	BITMAPFILEHEADER bmpFileHdr;
	//CFileDialog myDLG(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	//	"ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");
	//if (myDLG.DoModal() != IDOK)return image;

	//filename = myDLG.GetPathName();
	if (!file.Open(fileName, CFile::modeRead | CFile::typeBinary)) {
		return image;
	}
	file.Read(&bmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));
	if (myBmpInfoHdr.biBitCount >= 16) {
		bmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		bmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(bmpInfo->bmiColors,
			1i64 << (myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}
	bmpInfo->bmiHeader = myBmpInfoHdr;
	file.Seek(bmpFileHdr.bfOffBits, CFile::begin);
	image_size = bmpFileHdr.bfSize - bmpFileHdr.bfOffBits;
	bmpImage = new unsigned char[image_size];
	file.Read(bmpImage, (UINT)image_size);
	file.Close();
	image = new Image(bmpInfo->bmiHeader.biHeight, bmpInfo->bmiHeader.biWidth);
	if (myBmpInfoHdr.biBitCount == 24) {
		for (i = 0; i < bmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < bmpInfo->bmiHeader.biWidth; j++) {
				image->B[i][j] = bmpImage[((bmpInfo->bmiHeader.biHeight - 1 - i) * bmpInfo->bmiHeader.biWidth + j) * 3];
				image->G[i][j] = bmpImage[((bmpInfo->bmiHeader.biHeight - 1 - i) * bmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				image->R[i][j] = bmpImage[((bmpInfo->bmiHeader.biHeight - 1 - i) * bmpInfo->bmiHeader.biWidth + j) * 3 + 2];
			}
		}
	}
	else {
	}

	return image;
}

void CSampleDlg::OnKido()
{
	int i, j;
	double dy;

	//輝度化処理
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {

			dy = std::roundf(0.3 * m_Image->R[i][j] + 0.59 * m_Image->G[i][j] + 0.11 * m_Image->B[i][j]);
			m_ChangeImage->R[i][j] = (unsigned char)dy;
			m_ChangeImage->G[i][j] = (unsigned char)dy;
			m_ChangeImage->B[i][j] = (unsigned char)dy;

		}
	}

	UpdateImage();
}

void CSampleDlg::OnEdgeSearch()
{
	OnKido();

	int filter[3][3] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1} };

	for (int i = 0; i < m_Image->Height; i++) {
		for (int j = 0; j < m_Image->Width; j++) {
			int up = (i - 1) >= 0 ? i - 1 : 0;
			int down = (i + 1) <= m_Image->Height - 1 ? i + 1 : m_Image->Height - 1;
			int left = (j - 1) >= 0 ? j - 1 : 0;
			int right = (j + 1) <= m_Image->Width - 1 ? j + 1 : m_Image->Width - 1;
			int result = 0;

			result = m_Image->B[up][left] * filter[0][0]
				+ m_Image->B[up][j] * filter[0][1]
				+ m_Image->B[up][right] * filter[0][2]
				+ m_Image->B[i][left] * filter[1][0]
				+ m_Image->B[i][j] * filter[1][1]
				+ m_Image->B[i][right] * filter[1][2]
				+ m_Image->B[down][left] * filter[2][0]
				+ m_Image->B[down][j] * filter[2][1]
				+ m_Image->B[down][right] * filter[2][2];

			if (result < 0)
				result = 0;
			else if (result > 255)
				result = 255;
			m_ChangeImage->B[i][j] = result;
		}
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j] = m_ChangeImage->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j] = m_ChangeImage->G[i][j] = m_ChangeImage->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j] = m_ChangeImage->R[i][j] = m_ChangeImage->B[i][j];
		}
	}

	//再描画
	Invalidate();
}

void CSampleDlg::OnChangeImageFormat()
{
	/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
		//パラメータ
	int max = 2300;
	CString ary[] = { "cut_", "result_", "remove_" };
	CString savePath = IWI_PATH + "変換\\";
	//CString savePath = IWI_PATH + "実験\\2-6\\表示精度\\_立ち_-40\\";

	/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


	CDC* pDC = ((CStatic*)GetDlgItem(IDC_PICBOX))->GetDC();
	CString inputPath;
	CString outputPath;
	CImage* img = new CImage();

	inputPath.Format("%sa.jpg", IWI_PATH);
	outputPath.Format("%sB.bmp", IWI_PATH);
	img->Load(inputPath);
	img->Save(outputPath);
	img->Destroy();

	//BMPファイルの読み込み
	//変数の宣言
	int i, j;
	size_t image_size;
	CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;

	//取得したファイルをオープンする
	filename.Format(outputPath);
	if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

	//----------------------------------------------ファイル内容の読み込み処理

	//ファイルヘッダ部とインフォヘッダ部を読み込む
	file.Read(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	//前回の画像イメージをいったん解放
	if (m_BmpImage)delete[] m_BmpImage;
	//前回使用していれば、いったんメモリ解放する
	if (m_BmpInfo)delete[] m_BmpInfo;
	//前回の画像処理メモリをいったん解放
	if (m_Image)delete[] m_Image;
	if (m_ChangeImage)delete[] m_ChangeImage;

	//色情報を取得する
	//biBitCountは1ピクセルあたりのカラー表現ビット数
	//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//16ビット以上と未満でカラーデータ格納が異なる
	if (myBmpInfoHdr.biBitCount >= 16) {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(m_BmpInfo->bmiColors,
			1i64 << (myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}

	//m_BmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	m_BmpInfo->bmiHeader = myBmpInfoHdr;

	//ファイル内のビットマップ実データ位置に合わせる
	file.Seek(m_BmpFileHdr.bfOffBits, CFile::begin);

	//実データ分のバイト数を確保
	//bfsize	:ビットマップファイル全サイズ
	//bfffBits	:先頭にあるヘッダ情報サイズ
	image_size = m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits;
	m_BmpImage = new unsigned char[image_size];

	//ビットマップ実データを読み込み格納
	file.Read(m_BmpImage, (UINT)image_size);

	file.Close();

	//画像処理用のメモリを動的に確保
	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	m_ChangeImage = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {//フルカラーの場合
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->B[m_BmpInfo->bmiHeader.biHeight - i - 1][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
				m_Image->G[m_BmpInfo->bmiHeader.biHeight - i - 1][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				m_Image->R[m_BmpInfo->bmiHeader.biHeight - i - 1][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
			}
		}
	}
	filename.Format("BB");
	SaveImage(m_Image, filename, IWI_PATH);














	//for (CString name : ary) {
	//	for (int k = 10; k <= max; k += 10) {
	//		inputPath.Format("%s%s%s%d%s", IWI_PATH, "新規\\", name, k, ".jpg");
	//		outputPath.Format("%s%s%s%d%s", IWI_PATH, "変換\\", name, k, ".bmp");
	//		img->Load(inputPath);
	//		img->Save(outputPath);
	//		img->Destroy();

	//		//BMPファイルの読み込み
	//			//変数の宣言
	//		int i, j;
	//		size_t image_size;
	//		CString filename;
	//		CFile file;
	//		BITMAPINFOHEADER myBmpInfoHdr;

	//		//取得したファイルをオープンする
	//		filename.Format(outputPath);
	//		if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
	//			return;
	//		}

	//		//----------------------------------------------ファイル内容の読み込み処理

	//		//ファイルヘッダ部とインフォヘッダ部を読み込む
	//		file.Read(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	//		file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	//		//前回の画像イメージをいったん解放
	//		if (m_BmpImage)delete[] m_BmpImage;
	//		//前回使用していれば、いったんメモリ解放する
	//		if (m_BmpInfo)delete[] m_BmpInfo;
	//		//前回の画像処理メモリをいったん解放
	//		if (m_Image)delete[] m_Image;
	//		if (m_ChangeImage)delete[] m_ChangeImage;

	//		//色情報を取得する
	//		//biBitCountは1ピクセルあたりのカラー表現ビット数
	//		//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//		//16ビット以上と未満でカラーデータ格納が異なる
	//		if (myBmpInfoHdr.biBitCount >= 16) {
	//			m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	//		}
	//		else {
	//			m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
	//				(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
	//			file.Read(m_BmpInfo->bmiColors,
	//				1i64 << (myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	//		}

	//		//m_BmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	//		m_BmpInfo->bmiHeader = myBmpInfoHdr;

	//		//ファイル内のビットマップ実データ位置に合わせる
	//		file.Seek(m_BmpFileHdr.bfOffBits, CFile::begin);

	//		//実データ分のバイト数を確保
	//		//bfsize	:ビットマップファイル全サイズ
	//		//bfffBits	:先頭にあるヘッダ情報サイズ
	//		image_size = m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits;
	//		m_BmpImage = new unsigned char[image_size];

	//		//ビットマップ実データを読み込み格納
	//		file.Read(m_BmpImage, (UINT)image_size);

	//		file.Close();

	//		//画像処理用のメモリを動的に確保
	//		m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	//		m_ChangeImage = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	//		//RGBに分けて格納する
	//		if (myBmpInfoHdr.biBitCount == 24) {//フルカラーの場合
	//			for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
	//				for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
	//					m_Image->B[m_BmpInfo->bmiHeader.biHeight - i - 1][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
	//					m_Image->G[m_BmpInfo->bmiHeader.biHeight - i - 1][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
	//					m_Image->R[m_BmpInfo->bmiHeader.biHeight - i - 1][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
	//				}
	//			}
	//		}

	//		filename.Format("%s%d", name, k);
	//		SaveImage(m_Image, filename, savePath);
	//	}
	//}
}

void CSampleDlg::OnGetImage()
{
	if (!videoCapture.isOpened())
		if(!initCamera())
			return;
		
	//カメラから取得
	videoCapture.read(input);

	Mat showResult;
	resize(input, showResult, Size(), 0.5, 0.5);

	imshow("show", showResult);

	////RGB格納
	//for (int i = 0; i < HEIGHT; i++) {
	//	for (int j = 0; j < WIDTH; j++) {
	//		m_ChangeImage->B[i][j] = input.ptr(i)[j * 3];
	//		m_ChangeImage->G[i][j] = input.ptr(i)[j * 3 + 1];
	//		m_ChangeImage->R[i][j] = input.ptr(i)[j * 3 + 2];
	//	}
	//}

	//UpdateImage();
}

bool CSampleDlg::initCamera() {
	//タイトルバーにファイル名を表示する
	SetWindowText("カメラ");

	// 前回の画像イメージをいったん開放
	if (m_BmpImage) delete[] m_BmpImage;

	if (m_Image) delete[] m_Image;
	if (m_ChangeImage) delete[] m_ChangeImage;
	if (m_BaseImage) delete[] m_BaseImage;

	//前回使用していれば、いったんメモリ開放する
	if (m_BmpInfo) delete[] m_BmpInfo;
	m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];

	getBITMAPINFO();//ビットマップファイルヘッダ

	getBITMAPFILEHEADER();//ビットマップインフォの設定

	//画像処理用のメモリを動的に確保
	m_Image = new Image(HEIGHT, WIDTH);
	m_ChangeImage = new Image(HEIGHT, WIDTH);


	//Mat型の初期化
	input = Mat::zeros(Size(WIDTH, HEIGHT), CV_8UC3);

	// カメラからのビデオキャプチャを初期化する
	videoCapture.open(1);
	videoCapture.set(CAP_PROP_FRAME_HEIGHT, HEIGHT);
	videoCapture.set(CAP_PROP_FRAME_WIDTH, WIDTH);

	CString basePath = IWI_PATH + "BaseBoard.bmp";
	m_BaseImage = OpenImage(basePath);

	return true;
}



void CSampleDlg::OnCameraStart()
{
	//initCamera();
	m_timerID = SetTimer(1, 500, NULL);
}

void CSampleDlg::OnCameraStop()
{
	if (m_timerID != 0) {
		bool error = KillTimer(m_timerID);
		if (!error) {
			AfxMessageBox(_T("not stop timer"));
		}
		m_timerID = 0;
	}
}


void CSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		OnGetImage();
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

int CSampleDlg::getBITMAPINFO() {
	DWORD bytesPerPixel = 24 / 8; // １画素当たりのバイト数

	// まず，走査線の正味のバイト数を計算する．
	DWORD lineSizeDW = bytesPerPixel * WIDTH;

	// lineSizeDW を実際のサイズ (DWORD 数) にするため，
	// sizeof(DWORD) で割る (小数部切り上げ)．
	lineSizeDW = (int)lineSizeDW / sizeof(DWORD);

	DWORD lineSize = lineSizeDW * sizeof(DWORD);
	DWORD imageSize = lineSize * HEIGHT;

	m_BmpImage = new unsigned char[imageSize];

	//ビットマップインフォの設定
	m_BmpInfo->bmiHeader.biHeight = HEIGHT;
	m_BmpInfo->bmiHeader.biWidth = WIDTH;
	m_BmpInfo->bmiHeader.biBitCount = 24;
	m_BmpInfo->bmiHeader.biSizeImage = imageSize;
	m_BmpInfo->bmiHeader.biClrImportant = 0;
	m_BmpInfo->bmiHeader.biClrUsed = 0;
	m_BmpInfo->bmiHeader.biCompression = 0;
	m_BmpInfo->bmiHeader.biPlanes = 1;
	m_BmpInfo->bmiHeader.biSize = 40;
	m_BmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_BmpInfo->bmiHeader.biYPelsPerMeter = 0;
	//

	return 0;
}

int CSampleDlg::getBITMAPFILEHEADER() {
	//ビットマップファイルヘッダの設定
	m_BmpFileHdr.bfType = 0x4d42; /* "BM" */
	m_BmpFileHdr.bfReserved1 = 0;
	m_BmpFileHdr.bfReserved2 = 0;

	m_BmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	m_BmpFileHdr.bfSize = m_BmpInfo->bmiHeader.biSizeImage + m_BmpFileHdr.bfOffBits;
	//
	return 0;
}

void CSampleDlg::OnChangeHSV()
{
	cutImageUseHSV(m_Image);
	UpdateImage();
}

void CSampleDlg::cutImageUseHSV(Image* image) {
	MyPoint maxP(0, 0);
	MyPoint minP(image->Height, image->Width);
	for (int i = 0; i < image->Height; i++) {
		for (int j = 0; j < image->Width; j++) {
			int H, S, V;
			int R = image->R[i][j];
			int G = image->G[i][j];
			int B = image->B[i][j];

			int max = std::max(R, std::max(G, B));
			int min = std::min(R, std::min(G, B));
			//if (max == R)
			//	(H = (G - B) * 60) / (max - min);
			//else if (max == G)
			//	(H = (B - R) * 60) / (max - min) + 120;
			//else
			//	(H = (R - G) * 60) / (max - min) + 240;

			S = (max - min);
			//V = max;

			if (S > 100) {
				if (i < minP.y)
					minP.y = i;
				if (j < minP.x)
					minP.x = j;
				if (maxP.y < i)
					maxP.y = i;
				if (maxP.x < j)
					maxP.x = j;
			}
			else {
			}

			//m_ChangeImage->B[i][j] = S;
			//m_ChangeImage->G[i][j] = S;
			//m_ChangeImage->R[i][j] = S;
		}
	}

	image->cutRect(minP, maxP);
}

void CSampleDlg::OnBaseDiff()
{
	OnGetImage();
	baseDiff();
	UpdateImage();
}

void CSampleDlg::baseDiff() {
	for (int i = 0; i < m_Image->Height; i++) {
		for (int j = 0; j < m_Image->Width; j++) {
			float kido = std::roundf(0.3 * m_Image->R[i][j] + 0.59 * m_Image->G[i][j] + 0.11 * m_Image->B[i][j]);
			float baseKido = std::roundf(0.3 * m_BaseImage->R[i][j] + 0.59 * m_BaseImage->G[i][j] + 0.11 * m_BaseImage->B[i][j]);
			float diff = std::abs(kido - baseKido);

			if (diff < 50) {
				m_ChangeImage->B[i][j] = 0;
				m_ChangeImage->G[i][j] = 0;
				m_ChangeImage->R[i][j] = 0;
			}
		}
	}

	UpdateImage();
}

void CSampleDlg::OnSaveBaseBoard()
{
	OnGetImage();
	SaveImage(IWI_PATH + "BaseBoard");

	//Mat sub = input(Rect(Point(209, 30), Size(837, 543)));
	//imwrite(path + "sub.bmp", sub);
}

void CSampleDlg::OnOpening()
{
	int count = 1;
	Image* image = m_Image->copy();

	m_ChangeImage->contraction(count);
	UpdateImage();
	m_ChangeImage->expansion(image, count);
	UpdateImage();

	delete[] image;
}


void CSampleDlg::OnClosing()
{
	int count = 1;
	Image* image = m_Image->copy();

	m_ChangeImage->expansion(image, count);
	UpdateImage();
	m_ChangeImage->contraction(count);
	UpdateImage();

	delete[] image;
}


void CSampleDlg::OnCreateCutBoard()
{
	MyPoint minP = MyPoint(99999, 99999);
	MyPoint maxP = MyPoint(0, 0);

	for (int i = 0; i < m_Image->Height; i++) {
		for (int j = 0; j < m_Image->Width; j++) {
			int H = 0;
			int colors[3] = { m_Image->R[i][j], m_Image->G[i][j], m_Image->B[i][j] };
			int maxC = max(colors[0], max(colors[1], colors[2]));
			int minC = min(colors[0], min(colors[1], colors[2]));
			H = maxC - minC;

			if (H > 50) {
				if (minP.x > j)
					minP.x = j;
				else if (maxP.x < j)
					maxP.x = j;
				if (minP.y > i)
					minP.y = i;
				else if (maxP.y < i)
					maxP.y = i;
			}
		}
	}

	int w = maxP.x - minP.x;
	int h = maxP.y - minP.y;

	Image* cut = new Image(h, w);

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			cut->B[i][j] = m_Image->B[i + minP.y][j + minP.x];
			cut->G[i][j] = m_Image->G[i + minP.y][j + minP.x];
			cut->R[i][j] = m_Image->R[i + minP.y][j + minP.x];
		}
	}

	SaveImage(cut, "cutBoard");
}