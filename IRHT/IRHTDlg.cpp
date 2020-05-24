// IRHTDlg.cpp :  µœ÷Œƒº˛
//

#include "stdafx.h"
#include "IRHT.h"
#include "IRHTDlg.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ”√”⁄”¶”√≥Ã–Ú°∞πÿ”⁄°±≤Àµ•œ˚—ƒ CAboutDlg ∂‘ª∞øÅE

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ∂‘ª∞øÚ ˝æ›
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ÷ß≥÷

//  µœ÷
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIRHTDlg ∂‘ª∞øÅE




CIRHTDlg::CIRHTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIRHTDlg::IDD, pParent)
	, m_edit_classNum(3)
	, m_edit_K(200)
	, m_inputBackgroundType(0)
	, m_outputType(0)
	, m_inputFilePath(_T(""))
	, m_static_segmentationTimes(_T(""))
	, m_static_segmentationTimer(_T(""))
	, m_static_IRHTTimes(_T(""))
	, m_static_IRHTTimer(_T(""))
	, m_str_gaussians(_T("#\tMean\tVariance\r\n"))
	, m_str_irhtResults(_T("Xc\tYc\ta\tb\tphi\tT\tN\r\n"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIRHTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ClassNum, m_edit_classNum);
	DDX_Text(pDX, IDC_EDIT_KIRHT, m_edit_K);
	DDX_Text(pDX, IDC_EDIT_InputFile, m_inputFilePath);
	DDX_Text(pDX, IDC_STATIC_SegmentationTimes, m_static_segmentationTimes);
	DDX_Text(pDX, IDC_STATIC_SegmentationTimer, m_static_segmentationTimer);
	DDX_Text(pDX, IDC_STATIC_IRHTTimes, m_static_IRHTTimes);
	DDX_Text(pDX, IDC_STATIC_IRHTTimer, m_static_IRHTTimer);
	DDX_Text(pDX, IDC_EDIT_Gaussians, m_str_gaussians);
	DDX_Text(pDX, IDC_EDIT_IRHTResults, m_str_irhtResults);
	DDX_Control(pDX, IDC_CHECK_Skeleton, m_check_skeleton);
	DDX_Control(pDX, IDC_BUTTON_IRHT, m_button_irht);
	DDX_Control(pDX, IDC_BUTTON_RHT, m_button_rht);
	DDX_Control(pDX, IDC_BUTTON_WindowsClear, m_button_clear);
	DDX_Control(pDX, IDC_BUTTON_Snake, m_button_snake);
	DDX_Control(pDX, IDC_COMBO_BackgroundType, m_combo_inputBackgroundType);
	DDX_Control(pDX, IDC_COMBO_OutputType, m_combo_outputType);
	DDX_Control(pDX, IDC_CHECK_BilateralFilter, m_check_biFilter);
	DDX_Control(pDX, IDC_CHECK_Head, m_check_headFlag);
}

BEGIN_MESSAGE_MAP(CIRHTDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OpenFile, &CIRHTDlg::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_KMeans, &CIRHTDlg::OnBnClickedButtonKmeans)
	ON_BN_CLICKED(IDC_BUTTON_Fcm, &CIRHTDlg::OnBnClickedButtonFcm)
	ON_BN_CLICKED(IDC_BUTTON_IRHT, &CIRHTDlg::OnBnClickedButtonIrht)
	ON_BN_CLICKED(IDC_BUTTON_RHT, &CIRHTDlg::OnBnClickedButtonRht)
	ON_BN_CLICKED(IDC_BUTTON_WindowsClear, &CIRHTDlg::OnBnClickedButtonWindowsclear)
	ON_BN_CLICKED(IDC_BUTTON_Snake, &CIRHTDlg::OnBnClickedButtonSnake)
	ON_EN_CHANGE(IDC_EDIT_ClassNum, &CIRHTDlg::OnEnChangeEditClassnum)
	ON_EN_CHANGE(IDC_EDIT_KIRHT, &CIRHTDlg::OnEnChangeEditKirht)
	ON_CBN_SELCHANGE(IDC_COMBO_BackgroundType, &CIRHTDlg::OnCbnSelchangeComboBackgroundtype)
	ON_EN_CHANGE(IDC_EDIT_InputFile, &CIRHTDlg::OnEnChangeEditInputfile)
	ON_STN_CLICKED(IDC_STATIC_SegmentationTimer, &CIRHTDlg::OnStnClickedStaticSegmentationtimer)
	ON_BN_CLICKED(IDC_CHECK_Skeleton, &CIRHTDlg::OnBnClickedCheckSkeleton)
	ON_BN_CLICKED(IDC_BUTTON_EdgeDetection, &CIRHTDlg::OnBnClickedButtonEdgedetection)
	ON_BN_CLICKED(IDC_CHECK_Head, &CIRHTDlg::OnBnClickedCheckHead)
	ON_BN_CLICKED(IDC_BUTTON_iIRHT1, &CIRHTDlg::OnBnClickedButtoniirht1)
	ON_BN_CLICKED(IDC_BUTTON_iIRHT2, &CIRHTDlg::OnBnClickedButtoniirht2)
END_MESSAGE_MAP()


// CIRHTDlg œ˚œ¢¥¶¿˙œÃ–ÅE

BOOL CIRHTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Ω´°∞πÿ”⁄...°±≤Àµ•œ˚Ë˙ÿ”µΩœµÕ≥≤Àµ•÷–°£

	// IDM_ABOUTBOX ±ÿ–ÅE⁄œµÕ≥√ÅE˚”∂Œßƒ⁄°£
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// …Ë÷√¥À∂‘ª∞øÚµƒÕº±Í°£µ±”¶”√≥Ã–Ú÷˜¥∞ø⁄≤ª «∂‘ª∞øÚ ±£¨øÚº‹Ω´◊‘∂Ø
	//  ÷¥––¥À≤Ÿ◊ÅE
	SetIcon(m_hIcon, TRUE);			// …Ë÷√¥ÛÕº±ÅE
	SetIcon(m_hIcon, FALSE);		// …Ë÷√–°Õº±ÅE

	// TODO: ‘⁄¥ÀÃ˙ÿ”∂˚È‚µƒ≥ı ºªØ¥˙¬ÅE
	m_img = new ImageClass();

	m_irhtType = 0;

	m_img->SetSkeletonFlag(TRUE);
	m_img->SetHeadDectionFlag(TRUE);

	m_combo_inputBackgroundType.SetCurSel(0);
	m_combo_outputType.SetCurSel(0);
	m_check_skeleton.SetCheck(1);
	m_check_headFlag.SetCheck(1);
	m_check_biFilter.SetCheck(1);
	UpdateData(FALSE);

	return TRUE;  // ≥˝∑«Ω´Ωπµ„…Ë÷√µΩøÿº˛£¨∑Ò‘Ú∑µªÿ TRUE
}

void CIRHTDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// »Áπ˚œÚ∂‘ª∞øÚÃ˙ÿ”◊˚Ï°ªØ∞¥≈•£¨‘Ú–Ë“™œ¬√Êµƒ¥˙¬ÅE
//  ¿¥ªÊ÷∆∏√Õº±Í°£∂‘”⁄ π”√Œƒµµ/ ”Õºƒ£–Õµƒ MFC ”¶”√≥Ã–Ú£¨
//  ’‚Ω´”…øÚº‹◊‘∂ØÕÅE…°£

void CIRHTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ”√”⁄ªÊ÷∆µƒ…Ë±∏…œœ¬Œƒ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//  πÕº±ÅE⁄π§◊˜«¯æÿ–Œ÷–æ”÷–
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ªÊ÷∆Õº±ÅE
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//µ±”√ªßÕœ∂Ø◊˚Ï°ªØ¥∞ø⁄ ±œµÕ≥µ˜”√¥À∫Ø ˝»°µ√π‚±ÅE
//œ‘ æ°£
HCURSOR CIRHTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIRHTDlg::OnBnClickedButtonOpenfile()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	CString szFileters("JPEG Files(*.jpg;*.jpeg;*.jpe)|*.jpg;*.jpeg;*.jpe|BMP Files(*.bmp)|*.bmp|All Files (*.*)|*.*||");

	//CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, szFileters, this);

	CFileDialog myDlg(TRUE, _T("*.jpg;*.jpeg;*.jpe"), NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, szFileters, this);

	if(myDlg.DoModal() == IDOK)
	{
		m_inputFilePath = myDlg.GetPathName();  //ªÒµ√Œƒº˛ MessageBox(m_edit_inputFilepath);¬∑æ∂√˚√ª”–∞ÅE®¿©’π√ÅE
		UpdateData(FALSE);

		// read image by OpenCV
		/*strcpy_s(m_img->m_filePath, (LPCTSTR)(m_inputFilePath));
		strcpy_s(m_img->m_folderPath, (LPCTSTR)(myDlg.GetFolderPath()));*/
		/*m_img->m_filePath = m_inputFilePath.GetBuffer(m_inputFilePath.GetLength());
		m_img->m_folderPath = (myDlg.GetFolderPath()).GetBuffer((myDlg.GetFolderPath()).GetLength());*/

		m_img->m_filePath = new char[m_inputFilePath.GetLength() + 1];
		for (int i = 0; i <= m_inputFilePath.GetLength(); i++)
		{
			m_img->m_filePath[i] = m_inputFilePath.GetAt(i);
		}
		//m_img->m_filePath[i] = '\0';

		m_img->m_folderPath = new char[(myDlg.GetFolderPath()).GetLength() + 1];
		for (int i = 0; i <= (myDlg.GetFolderPath()).GetLength(); i++)
		{
			m_img->m_folderPath[i] = (myDlg.GetFolderPath()).GetAt(i);
		}
		//m_img->m_folderPath[size2] = '\0';

		cvDestroyAllWindows();

		if (m_check_skeleton.GetCheck() == 1)
			m_img->SetSkeletonFlag(TRUE);
		else
			m_img->SetSkeletonFlag(FALSE);

		if (m_check_headFlag.GetCheck() == 1)
			m_img->SetHeadDectionFlag(TRUE);
		else
			m_img->SetHeadDectionFlag(FALSE);

		m_img->LoadImg(m_check_biFilter.GetCheck());

		if( m_img == NULL ) 
		{
			AfxMessageBox (_T("Image Load Failed!"));
			return;
		}
		else
		{
			cvNamedWindow ("InputImage", CV_WINDOW_AUTOSIZE);
			//cvMoveWindow("InputImage", 400, 100);
			cvShowImage ("InputImage", m_img->m_in_img);
			//cvWaitKey (0);

			m_str_gaussians = "#\tMean\tVariance\r\n";
			m_button_irht.EnableWindow(FALSE);
			m_button_rht.EnableWindow(FALSE);
			m_button_clear.EnableWindow(FALSE);
			m_button_snake.EnableWindow(FALSE);
			UpdateData(FALSE);
		}
	}
}

void CIRHTDlg::OnBnClickedButtonKmeans()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	int k;
	bool s_flag;

	if (m_img->m_in_img == NULL)
	{
		AfxMessageBox(_T("Image has not been load!"));
		return;
	}

	ini_timer.Reset();                                         // reset timer

	s_flag = true;

	m_img->Init(m_edit_classNum, m_inputBackgroundType);
	m_img->InitOutImg();

	if (m_check_skeleton.GetCheck() == 1)
		m_img->SetSkeletonFlag(TRUE);
	else
		m_img->SetSkeletonFlag(FALSE);

	if (m_check_headFlag.GetCheck() == 1)
		m_img->SetHeadDectionFlag(TRUE);
	else
		m_img->SetHeadDectionFlag(FALSE);

	if (m_check_skeleton.GetCheck() == BST_CHECKED)
	{
		/*if (!m_img->InitClusterCenter())
		{
			AfxMessageBox(_T("InitialClusterCenter Failed!"));
			return;
		}*/

		m_img->m_mean[0] = 0;
		m_img->m_mean[1] = 100;
		m_img->m_mean[2] = 200;
	}
	else
	{
		m_img->m_mean[0] = 0;
		m_img->m_mean[1] = 100;
		//m_img->m_mean[2] = 200;
	}

	ini_timer.Start();

	while(s_flag)
	{
		//∏˘æ›æ€¿‡÷––ƒ£¨∞—∏˜∏ˆœÛÀÿµ„∑÷¿ÅE
		m_img->K_PixelClass();

		ini_timer.Stop();

		// ‰≥ˆÕºœÅE
		m_img->CreateOutputImg(0);

		cvNamedWindow("Result Of K-Mean", 1);
		cvShowImage("Result Of K-Mean", m_img->m_out_img);

		/* Continue timer */
		ini_timer.Start();

		s_flag = m_img->KMeans();
		++m_img->m_iterativeTimes;

		/* Do not count GUI overhead */
		ini_timer.Stop();

		// ‰≥ˆΩ·πÅE
		m_static_segmentationTimes.Format(_T("%d"), m_img->m_iterativeTimes);
		m_static_segmentationTimer.Format(_T("%d ms"), 0);
		UpdateData(false);
	}
	m_static_segmentationTimer.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());
	UpdateData(false);

	//º∆À„∑Ω≤ÅE
	m_img->UpdateVariances();

	// ‰≥ˆÕºœÅE
	m_img->CreateOutputImg(m_outputType);

	//œ‘ ææ˘÷µ”ÅEΩ≤ÅE
	m_str_gaussians = "#\tMean\tVariance\r\n";
	for(k = 0; k < m_img->m_classSum; k++)
		m_str_gaussians.AppendFormat(_T("%d\t%.2lf\t%.2lf\r\n"), k+1, FormatDouble(2, (m_img->m_mean[k])), FormatDouble(2, (m_img->m_variance[k])));
	UpdateData(false);

	// Show the results
	CString strValue, strNum;
	strValue = "Result Of K-Mean";
	if (m_outputType != 0)
	{
		if (m_outputType == 1)
			strValue += " : Gray Part";
		else
			strValue += " : Bright Part";
	}
	cvNamedWindow((char *)(LPCTSTR)strValue, 1);
	cvShowImage((char *)(LPCTSTR)strValue, m_img->m_out_img);
	cvSaveImage("Input\\Kmean.png", m_img->m_out_img);
	//cvWaitKey(0);

	//
	strValue = "Iterative Times : ";
	strNum.Format(_T("%d"), m_img->m_iterativeTimes);
	strValue += strNum;
	strValue += "\r\n";
	strValue += "Consuming Time : ";
	strNum.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());
	strValue += strNum;
	strValue += "\r\n";

	//AfxMessageBox(strValue);

	m_img->BrightPartExtraction();

	if (m_check_skeleton.GetCheck() == BST_CHECKED)
		m_img->SkeletonizationByDistanceTransform();
	else
		m_img->Skeletons();
	//m_img->SkeletonizationByThinning();

	m_button_irht.EnableWindow(TRUE);
	m_button_rht.EnableWindow(TRUE);
	m_button_clear.EnableWindow(TRUE);
}

void CIRHTDlg::OnBnClickedButtonFcm()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	int k;

	if (m_img->m_in_img == NULL)
	{
		AfxMessageBox(_T("Image has not been load!"));
		return;
	}

	ini_timer.Reset();                                         // reset timer

	m_img->Init(m_edit_classNum, m_inputBackgroundType);
	m_img->InitOutImg();

	if (m_check_skeleton.GetCheck() == 1)
		m_img->SetSkeletonFlag(TRUE);
	else
		m_img->SetSkeletonFlag(FALSE);

	if (m_check_headFlag.GetCheck() == 1)
		m_img->SetHeadDectionFlag(TRUE);
	else
		m_img->SetHeadDectionFlag(FALSE);

	if (!m_img->InitClusterCenter())
	{
		AfxMessageBox(_T("InitialClusterCenter Failed!"));
		return;
	}

	ini_timer.Start();

	m_img->FuzzyClustering(0, 0, 0);

	// ‰≥ˆΩ·πÅE
	m_static_segmentationTimes.Format(_T("%d"), m_img->m_iterativeTimes);
	m_static_segmentationTimer.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());
	UpdateData(false);

	//º∆À„∑Ω≤ÅE
	m_img->UpdateVariances();

	// ‰≥ˆÕºœÅE
	m_img->CreateOutputImg(m_outputType);

	//œ‘ ææ˘÷µ”ÅEΩ≤ÅE
	m_str_gaussians = "#\tMean\tVariance\r\n";
	for(k = 0; k < m_img->m_classSum; k++)
		m_str_gaussians.AppendFormat(_T("%d\t%.2lf\t%.2lf\r\n"), k+1, FormatDouble(2, (m_img->m_mean[k])), FormatDouble(2, (m_img->m_variance[k])));
	UpdateData(false);

	cvNamedWindow("SegmentationResults", 1);
	cvShowImage("SegmentationResults", m_img->m_out_img);

	/*m_img->BrightPartExtraction();

	if (m_check_skeleton.GetCheck() == BST_CHECKED)
	m_img->SkeletonizationByDistanceTransform();
	//m_img->SkeletonizationByThinning();

	m_button_irht.EnableWindow(TRUE);
	m_button_rht.EnableWindow(TRUE);
	m_button_clear.EnableWindow(TRUE);*/
}

void CIRHTDlg::OnBnClickedButtonEdgedetection()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	if (m_img->m_in_img == NULL)
	{
		AfxMessageBox(_T("Image has not been load!"));
		return;
	}

	m_img->Init(m_edit_classNum, m_inputBackgroundType);
	m_img->InitOutImg();

	if (m_check_skeleton.GetCheck() == 1)
		m_img->SetSkeletonFlag(TRUE);
	else
		m_img->SetSkeletonFlag(FALSE);

	if (m_check_headFlag.GetCheck() == 1)
		m_img->SetHeadDectionFlag(TRUE);
	else
		m_img->SetHeadDectionFlag(FALSE);

	m_img->MyCannyDetection();

	m_img->Skeletons();

	m_button_irht.EnableWindow(TRUE);
	m_button_rht.EnableWindow(TRUE);
	m_button_clear.EnableWindow(TRUE);
}

void CIRHTDlg::OnBnClickedButtonIrht()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	ini_timer.Reset();                                         // reset timer
	ini_timer.Start();                                         // start timer

	m_irhtType = 0;

	m_img->IRHT_Ini(m_edit_K);

	if (m_img->IRHT(m_irhtType))
	{
		ini_timer.Stop();

		m_img->CreateOutputEllipse();

		m_static_IRHTTimes.Format(_T("%d"), m_img->m_iterativeTimesIRHT);
		m_static_IRHTTimer.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());
		//m_str_irhtResults = "Xc\tYc\ta\tb\tphi\tN\r\n";
		m_str_irhtResults.AppendFormat(_T("%d\t%d\t%d\t%d\t%0.2f\t%d\t%d\r\n"), m_img->m_cEst.xc, m_img->m_cEst.yc, m_img->m_cEst.a, m_img->m_cEst.b, FormatDouble(2, m_img->m_cEst.phi), m_img->m_iterativeTimesIRHT, m_img->m_pointNumOnFinalEllipse);
		m_button_snake.EnableWindow(TRUE);

		UpdateData(FALSE);

		//output parameter c
		double c = sqrt((double)m_img->m_cEst.a*m_img->m_cEst.a - (double)m_img->m_cEst.b*m_img->m_cEst.b);

		CString str, str_temp;
		str = "(Xc, Yc) : ";
		str_temp.Format(_T("%d, "), m_img->m_cEst.xc);
		str += str_temp;
		str_temp.Format(_T("%d\na = "), m_img->m_cEst.yc);
		str += str_temp;
		str_temp.Format(_T("%d, b = "), m_img->m_cEst.a);
		str += str_temp;
		str_temp.Format(_T("%d, phi = "), m_img->m_cEst.b);
		str += str_temp;
		str_temp.Format(_T("%f\n"), m_img->m_cEst.phi);
		str += str_temp;
		str_temp.Format(_T("T = %d, "), m_img->m_iterativeTimesIRHT);
		str += str_temp;
		str_temp.Format(_T("N = %d"), m_img->m_pointNumOnFinalEllipse);
		str += str_temp;
		str_temp.Format(_T("\ne = %.2f"), c/m_img->m_cEst.a);
		str += str_temp;

		AfxMessageBox(str);
	}
	else
	{
		ini_timer.Stop();
		AfxMessageBox(_T("No valid ellipse!"));
	}
}

void CIRHTDlg::OnBnClickedButtoniirht1()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	m_irhtType = 1;
}

void CIRHTDlg::OnBnClickedButtoniirht2()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	m_irhtType = 2;
}

void CIRHTDlg::OnBnClickedButtonRht()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	ini_timer.Reset();                                         // reset timer
	ini_timer.Start();                                         // start timer

	m_img->IRHT_Ini(m_edit_K);

	if (m_img->RHT(0))
	{
		ini_timer.Stop();

		m_img->CreateOutputEllipse();

		m_static_IRHTTimer.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());

		UpdateData(FALSE);

		//output parameter c
		CString str, str_temp;
		str_temp.Format(_T("(Xc, Yc) : %d, %d\n"), m_img->m_cEst.xc, m_img->m_cEst.yc);
		str += str_temp;
		str_temp.Format(_T("a = %d, b = %d, phi = %f\n"), m_img->m_cEst.a, m_img->m_cEst.b, m_img->m_cEst.phi);
		str += str_temp;

		AfxMessageBox(str);
	}
	else
	{
		ini_timer.Stop();
		AfxMessageBox(_T("No valid ellipse!"));
	}
}

void CIRHTDlg::OnBnClickedButtonWindowsclear()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	cvDestroyAllWindows();
}

void CIRHTDlg::OnBnClickedButtonSnake()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	m_img->SetRingROI(m_img->m_cEst);
	//m_img->GetPointsOnEllipse(m_img->m_cEst);

	//m_img->SnakeDeformation(m_img->startPoints, m_img->startPointsNum);
}

double CIRHTDlg::FormatDouble(int n, double d)
{
	double temp;

	temp = d * pow((double)10.0, n) + 0.5;
	temp = ((int)temp) / pow((double)10.0, n);

	return temp;
}

void CIRHTDlg::OnEnChangeEditInputfile()
{
	// TODO:  »Áπ˚∏√øÿº˛ « RICHEDIT øÿº˛£¨‘ÚÀÅE´≤ªªÅE
	// ∑¢ÀÕ∏√Õ®÷™£¨≥˝∑«÷ÿ–¥ CDialog::OnInitDialog()
	// ∫Ø ˝≤¢µ˜”√ CRichEditCtrl().SetEventMask()£¨
	// Õ¨ ±Ω´ ENM_CHANGE ±ÅEæ°∞ªÚ°±‘ÀÀ„µΩ—⁄¬ÅE–°£

	// TODO:  ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);
}

void CIRHTDlg::OnEnChangeEditClassnum()
{
	// TODO:  »Áπ˚∏√øÿº˛ « RICHEDIT øÿº˛£¨‘ÚÀÅE´≤ªªÅE
	// ∑¢ÀÕ∏√Õ®÷™£¨≥˝∑«÷ÿ–¥ CDialog::OnInitDialog()
	// ∫Ø ˝≤¢µ˜”√ CRichEditCtrl().SetEventMask()£¨
	// Õ¨ ±Ω´ ENM_CHANGE ±ÅEæ°∞ªÚ°±‘ÀÀ„µΩ—⁄¬ÅE–°£

	// TODO:  ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);
}

void CIRHTDlg::OnEnChangeEditKirht()
{
	// TODO:  »Áπ˚∏√øÿº˛ « RICHEDIT øÿº˛£¨‘ÚÀÅE´≤ªªÅE
	// ∑¢ÀÕ∏√Õ®÷™£¨≥˝∑«÷ÿ–¥ CDialog::OnInitDialog()
	// ∫Ø ˝≤¢µ˜”√ CRichEditCtrl().SetEventMask()£¨
	// Õ¨ ±Ω´ ENM_CHANGE ±ÅEæ°∞ªÚ°±‘ÀÀ„µΩ—⁄¬ÅE–°£

	// TODO:  ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);
}

void CIRHTDlg::OnCbnSelchangeComboBackgroundtype()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);
}

void CIRHTDlg::OnStnClickedStaticSegmentationtimer()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);
}

void CIRHTDlg::OnBnClickedCheckSkeleton()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);

	if (m_check_skeleton.GetCheck() == 1)
		m_img->SetSkeletonFlag(TRUE);
	else
		m_img->SetSkeletonFlag(FALSE);
}

void CIRHTDlg::OnBnClickedCheckHead()
{
	// TODO: ‘⁄¥ÀÃ˙ÿ”øÿº˛Õ®÷™¥¶¿˙œÃ–Ú¥˙¬ÅE
	UpdateData(TRUE);

	if (m_check_headFlag.GetCheck() == 1)
		m_img->SetHeadDectionFlag(TRUE);
	else
		m_img->SetHeadDectionFlag(FALSE);
}


