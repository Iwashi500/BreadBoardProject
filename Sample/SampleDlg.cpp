
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
//#include "Vector.h"
//#include <opencv2/opencv.hpp>

//#include <opencv2/opencv.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

#define BPP 24
#define ICEIL(dividend, divisor) (((dividend) + ((divisor) - 1)) / (divisor))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

	check_fill = {};
	check_remove = {};
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

	if (m_OrgImage)
		delete[] m_OrgImage;
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
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON8, &CSampleDlg::OnTest)
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

	int debug = 0;
	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {//フルカラーの場合
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				if (j == 639)
					debug++;

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
	return SaveImage(image, fileName, IWI_PATH);
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

void CSampleDlg::OnTest()
{	
	CString fileName = "";
	int fileIndex = 0;

	fileName.Format(m_fileName);
	SaveImage(m_Image, fileName, RESULT_PATH);
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
Image* CSampleDlg::OpenImage() {
	int i, j;
	size_t image_size;
	CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;
	unsigned char* bmpImage;
	LPBITMAPINFO bmpInfo;
	Image* image = new Image(0, 0);
	BITMAPFILEHEADER bmpFileHdr;
	CFileDialog myDLG(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");
	if (myDLG.DoModal() != IDOK)return image;

	filename = myDLG.GetPathName();
	if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
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