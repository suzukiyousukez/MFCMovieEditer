
#pragma once
#include <opencv2/opencv.hpp>
#include "afxcmn.h"

class CMFCMovieEditerDlg : public CDialogEx
{
public:
	CMFCMovieEditerDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_MFCMOVIEEDITER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static UINT ThreadMovie(LPVOID pParam);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CString edit_box_string;
	bool movie_on;
	std::string file_path;
	afx_msg void OnBnClickedButton5();
	bool movie_close;
	bool movie_capture;

	int out_file_mode;
	CButton* start;
	CButton* stop;
	CButton* capture;
	CButton* close;
	CSliderCtrl slider_ctr;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int frame_count;
	bool slider_move;
};

