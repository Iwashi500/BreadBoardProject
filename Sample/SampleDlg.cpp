
// SampleDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "Sample.h"
#include "SampleDlg.h"
#include "afxdialogex.h"
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

#define _USE_MATH_DEFINES
#include <math.h>

#define BPP 24
#define ICEIL(dividend, divisor) (((dividend) + ((divisor) - 1)) / (divisor))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;

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
	std::list<MyPoint> points;
	MyPoint base(54, 20);
	MyPoint add(8, 8);

	for (int j = 0; j < 63; j++) {
		OnLine onLine;
		ChainPoint chain(base, MyPoint(base.y, base.x + add.x * j));
		for (int i = 0; i < 5; i++) {
			int y = base.y + add.y * i;
			int x = base.x + add.x * j;
			MyPoint point(y, x);
			m_Image->drawPoint(point, 3);
			chain.pushNext(point);
			onLine.push(chain);
		}
		breadBoard.push(onLine);
	}

	base.y = 110;
	for (int j = 0; j < 63; j++) {
		OnLine onLine;
		ChainPoint chain(base, MyPoint(base.y, base.x + add.x * j));
		for (int i = 0; i < 5; i++) {
			int y = base.y + add.y * i;
			int x = base.x + add.x * j;
			MyPoint point(y, x);
			m_Image->drawPoint(point, 3);
			chain.pushNext(point);
			onLine.push(chain);
		}
		breadBoard.push(onLine);
	}

	for (OnLine onLine : breadBoard.getOnLines()) {
		drawOnLine(onLine);
	}

	//base.y = 24;
	//base.x = 36;
	//for (int i = 0; i < 2; i++) {
	//	for (int j = 0; j < 59; j++) {
	//		int y = base.y + add.y * i;
	//		int x = base.x + add.x * j;
	//		drawPoint(MyPoint(y, x), 3);
	//	}
	//}

	//base.y = 165;
	//for (int i = 0; i < 2; i++) {
	//	for (int j = 0; j < 59; j++) {
	//		int y = base.y + add.y * i;
	//		int x = base.x + add.x * j;
	//		drawPoint(MyPoint(y, x), 3);
	//	}
	//}

	UpdateImage();
}

void CSampleDlg::OnTest()
{	
	//if (!videoCapture.isOpened())
	//	if (!initCamera())
	//		return;
	//videoCapture.read(input);

	String inputPath = RESULT_PATH + "input.bmp";
	input = imread(inputPath, 1);

	Mat result;

	//HSV
	Mat hsv;
	cvtColor(input, hsv, CV_BGR2HSV);

	//S範囲抽出
	Scalar sMin = Scalar(0, 200, 0);
	Scalar sMax = Scalar(180, 255, 255);
	Mat mask;
	inRange(hsv, sMin, sMax, mask);
	
	////エッジ検出
	//Mat edge;
	//Canny(mask, edge, 200, 255);

	//TODO: エッジ検出してからすべき
	//ハフ変換
	Mat hough;
	input.copyTo(hough);
	std::vector<Vec4i> lines;
	HoughLinesP(mask, lines, 1, CV_PI / 180.0, 150, 100, 10);
	std::vector<Vec4i>::iterator it = lines.begin();
	double sumAngle = 0;
	for (; it != lines.end(); ++it) {
		Vec4i l = *it;
		double x = l[2] - l[0];
		double y = l[3] - l[1];
		double angle = std::atan2(y, x) * 180 / M_PI;
		sumAngle += angle;
		cv::line(hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 255), 1);
	}

	//アフィン
	double angle = sumAngle / lines.size();
	double scale = 1.0;
	Mat affin;
	Mat affin_mask;
	Rect roi_rect(cvRound(input.cols * 0), cvRound(input.rows * 0), cvRound(input.cols * 1), cvRound(input.rows * 1));
	Point2d center(input.cols * 0.5, input.rows * 0.5);
	Mat affine_matrix = getRotationMatrix2D(center, angle, scale);

	warpAffine(input, affin, affine_matrix, affin.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar::all(0));
	warpAffine(mask, affin_mask, affine_matrix, affin.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar::all(0));

	//ハフ変換
	Mat hough2 = affin.clone();
	Point2d minP(INT_MAX, INT_MAX);
	Point2d maxP(0, 0);
	HoughLinesP(affin_mask, lines, 1, CV_PI / 180.0, 150, 100, 10);
	std::vector<Vec4i>::iterator it2 = lines.begin();
	for (; it2 != lines.end(); ++it2) {
		Vec4i l = *it2;
		double x = l[2] - l[0];
		double y = l[3] - l[1];
		double angle = std::atan2(y, x) * 180 / M_PI;
		cv::line(hough2, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 255), 1);

		if (minP.x > l[0])
			minP.x = l[0];
		if (maxP.x < l[0])
			maxP.x = l[0];
		if (minP.x > l[2])
			minP.x = l[2];
		if (maxP.x < l[2])
			maxP.x = l[2];

		if (minP.y > l[1])
			minP.y = l[1];
		if (maxP.y < l[1])
			maxP.y = l[1];
		if (minP.y > l[3])
			minP.y = l[3];
		if (maxP.y < l[3])
			maxP.y = l[3];
	}

	Size boardSize(maxP.x - minP.x, maxP.y - minP.y);
	Point2f boardCenter(minP.x + (boardSize.width / 2), minP.y + (boardSize.height / 2));
	Mat board;
	getRectSubPix(affin, boardSize, boardCenter, board);

	affin.copyTo(result, affin_mask);

	imshow("result", result);
	String path = RESULT_PATH;
	int index = 0;
	imwrite(path + format("%d_", ++index) + "input.bmp", input);
	imwrite(path + format("%d_", ++index) + "hsv.bmp", hsv);
	imwrite(path + format("%d_", ++index) + "mask.bmp", mask);
	//imwrite(path + format("%d_", ++index) + "edge.bmp", edge);
	imwrite(path + format("%d_", ++index) + "hough.bmp", hough);
	imwrite(path + format("%d_", ++index) + "affin.bmp", affin);
	imwrite(path + format("%d_", ++index) + "affin_mask.bmp", affin_mask);
	imwrite(path + format("%d_", ++index) + "hough2.bmp", hough2);
	imwrite(path + format("%d_", ++index) + "board.bmp", board);
	imwrite(path + format("%d_", ++index) + "result.bmp", result);
}

void CSampleDlg::drawOnLine(OnLine line) {
	for (ChainPoint chain : line.getChains()) {
		m_Image->drawLine(chain.point1, chain.point2);
	}
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

	//RGB格納
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			m_ChangeImage->B[i][j] = input.ptr(i)[j * 3];
			m_ChangeImage->G[i][j] = input.ptr(i)[j * 3 + 1];
			m_ChangeImage->R[i][j] = input.ptr(i)[j * 3 + 2];
		}
	}

	UpdateImage();
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
	videoCapture.open(0);
	videoCapture.set(CAP_PROP_FRAME_HEIGHT, HEIGHT);
	videoCapture.set(CAP_PROP_FRAME_WIDTH, WIDTH);

	CString basePath = IWI_PATH + "BaseBoard.bmp";
	m_BaseImage = OpenImage(basePath);

	return true;
}



void CSampleDlg::OnCameraStart()
{
	//initCamera();
	m_timerID = SetTimer(1, 100, NULL);
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
		//OnChangeHSV();
		baseDiff();
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
