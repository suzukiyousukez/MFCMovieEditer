
#include "stdafx.h"
#include "MFCMovieEditer.h"
#include "MFCMovieEditerDlg.h"
#include "afxdialogex.h"

using namespace cv;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CMFCMovieEditerDlg::CMFCMovieEditerDlg(CWnd* pParent)
	: CDialogEx(CMFCMovieEditerDlg::IDD, pParent)
	, edit_box_string(_T(""))
	, out_file_mode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	frame_count = 0;
}

//データ対応
void CMFCMovieEditerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, edit_box_string);
	DDX_Radio(pDX, IDC_RADIO1, out_file_mode);
	DDX_Control(pDX, IDC_SLIDER1, slider_ctr);
}

BEGIN_MESSAGE_MAP(CMFCMovieEditerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCMovieEditerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCMovieEditerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCMovieEditerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCMovieEditerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCMovieEditerDlg::OnBnClickedButton5)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

//初期化
BOOL CMFCMovieEditerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	// TODO: 初期化をここに追加します。
	start = (CButton*)GetDlgItem(IDC_BUTTON2);
	stop = (CButton*)GetDlgItem(IDC_BUTTON3);
	capture = (CButton*)GetDlgItem(IDC_BUTTON4);
	close = (CButton*)GetDlgItem(IDC_BUTTON5);

	stop->EnableWindow(FALSE);
	capture->EnableWindow(FALSE);
	close->EnableWindow(FALSE);
	frame_count = 0;

	return TRUE;
}

void CMFCMovieEditerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMFCMovieEditerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMFCMovieEditerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//動画表示スレッド
UINT CMFCMovieEditerDlg::ThreadMovie(LPVOID pParam)
{
	CMFCMovieEditerDlg* pDlg = (CMFCMovieEditerDlg*)pParam;
	
	VideoCapture video;
	video.open(pDlg->file_path);

	if (!video.isOpened())return 0;

	Mat frame;
	char str[20];

	while (1){
		//CLOSE
		if (pDlg->movie_close){
			break;
		}
		//STOP
		else if (!pDlg->movie_on){
			//CAPTURE
			if (pDlg->movie_capture){
				pDlg->UpdateData(TRUE);
				if(pDlg->out_file_mode==0)sprintf_s(str, 20, "./%d.jpg", pDlg->frame_count);
				else sprintf_s(str, 20, "./%d.png", pDlg->frame_count);
				imwrite(str, frame);
				pDlg->movie_capture = false;
			}
		}
		//START
		else{	
			if (pDlg->slider_move){
				video.set(CV_CAP_PROP_POS_FRAMES, pDlg->frame_count);
				pDlg->slider_move = false;
			}
			
			video >> frame;
			if (frame.empty())break;
			namedWindow(pDlg->file_path, CV_WINDOW_NORMAL);
			imshow(pDlg->file_path, frame);
			waitKey(30);
			pDlg->frame_count++;
			pDlg->slider_ctr.SetPos(pDlg->frame_count);
		}
	}
	
	destroyAllWindows();
	video.release();

	pDlg->movie_close = true;
	pDlg->start->EnableWindow(TRUE);
	pDlg->stop->EnableWindow(FALSE);
	pDlg->capture->EnableWindow(FALSE);
	pDlg->close->EnableWindow(FALSE);
	return 0;
}

//ファイル選択
void CMFCMovieEditerDlg::OnBnClickedButton1()
{
	CString filter("Movie Files (*.mp4;*.avi)|*.mp4; *.avi||");
	CFileDialog selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);

	if (selDlg.DoModal() == IDOK)
	{
		edit_box_string = selDlg.GetPathName();
		file_path = static_cast<std::string>(edit_box_string);

		UpdateData(FALSE);
	}
}

//開始
void CMFCMovieEditerDlg::OnBnClickedButton2()
{
	VideoCapture video(file_path);
	int frame_total = video.get(CV_CAP_PROP_FRAME_COUNT);
	video.release();
	slider_ctr.SetRange(0, frame_total);

	movie_on = true;
	movie_close = false;
	start->EnableWindow(FALSE);
	stop->EnableWindow(TRUE);
	close->EnableWindow(TRUE);
	AfxBeginThread(ThreadMovie, this);
}

//停止
void CMFCMovieEditerDlg::OnBnClickedButton3()
{
	movie_on = !movie_on;
	capture->EnableWindow(TRUE);
}

//キャプチャ
void CMFCMovieEditerDlg::OnBnClickedButton4()
{
	movie_capture = true;
}

//閉じる
void CMFCMovieEditerDlg::OnBnClickedButton5()
{
	movie_close = true;
	start->EnableWindow(TRUE);
	stop->EnableWindow(FALSE);
	capture->EnableWindow(FALSE);
	close->EnableWindow(FALSE);
}

//スライダ動作時
void CMFCMovieEditerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (*pScrollBar == slider_ctr){
		frame_count = slider_ctr.GetPos();
	}

	slider_move = true;
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


