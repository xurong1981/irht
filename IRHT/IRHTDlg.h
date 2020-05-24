// IRHTDlg.h : Õ∑Œƒº˛
//

#pragma once

#include "stdlib.h"

// Macro definitions

/* Timer classes */
#include "CKProcessTimeCounter.h"

static CKProcessTimeCounter timer("core");       // CPU timer
static CKProcessTimeCounter ini_timer("core");       // CPU timer

//OpenCV Library
#include "opencv\cv.h"
#include "opencv\highgui.h"

// Defined by myself
#include "ImageClass.h"
#include "afxwin.h"


// CIRHTDlg ∂‘ª∞øÅE
class CIRHTDlg : public CDialog
{
// ππ‘ÅE
public:
	CIRHTDlg(CWnd* pParent = NULL);	// ±ÅEºππ‘ÅEØ ˝

// ∂‘ª∞øÚ ˝æ›
	enum { IDD = IDD_IRHT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ÷ß≥÷


//  µœ÷
protected:
	HICON m_hIcon;

	// …˙≥…µƒœ˚œ¢”≥…‰∫Ø ˝
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	ImageClass *m_img;

	int m_edit_classNum;
	int m_edit_K;
	int m_inputBackgroundType;
	int m_outputType;
	int m_irhtType;
	CButton m_check_skeleton;
	CButton m_check_biFilter;
	CButton m_check_headFlag;
	CString m_inputFilePath;
	CString m_static_segmentationTimes;
	CString m_static_segmentationTimer;
	CString m_static_IRHTTimes;
	CString m_static_IRHTTimer;
	CString m_str_gaussians;
	CString m_str_irhtResults;

	CButton m_button_irht;
	CButton m_button_rht;
	CButton m_button_clear;
	CButton m_button_snake;
	CComboBox m_combo_outputType;
	CComboBox m_combo_inputBackgroundType;
	
public:
	double FormatDouble(int n, double d);

public:
	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonKmeans();
	afx_msg void OnBnClickedButtonFcm();
	afx_msg void OnBnClickedButtonIrht();
	afx_msg void OnBnClickedButtonRht();
	afx_msg void OnBnClickedButtonWindowsclear();
	afx_msg void OnBnClickedButtonSnake();
	afx_msg void OnEnChangeEditClassnum();
	afx_msg void OnEnChangeEditKirht();
	afx_msg void OnCbnSelchangeComboBackgroundtype();
	afx_msg void OnEnChangeEditInputfile();
	afx_msg void OnStnClickedStaticSegmentationtimer();
	afx_msg void OnBnClickedCheckSkeleton();
	afx_msg void OnBnClickedButtonEdgedetection();
	afx_msg void OnBnClickedCheckHead();
	afx_msg void OnBnClickedButtoniirht1();
	afx_msg void OnBnClickedButtoniirht2();
};
