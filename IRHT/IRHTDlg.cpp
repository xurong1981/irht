// IRHTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IRHT.h"
#include "IRHTDlg.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单消涯 CAboutDlg 对话縼E

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CIRHTDlg 对话縼E




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


// CIRHTDlg 消息处历咸衼E

BOOL CIRHTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单消楮赜到系统菜单中。

	// IDM_ABOUTBOX 必衼E谙低趁丒段凇�
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操讈E
	SetIcon(m_hIcon, TRUE);			// 设置大图眮E
	SetIcon(m_hIcon, FALSE);		// 设置小图眮E

	// TODO: 在此铁赜尔殁的初始化代聛E
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框铁赜臁化按钮，则需要下面的代聛E
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动蛠E伞�

void CIRHTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图眮E诠ぷ髑匦沃芯又�
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图眮E
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动臁化窗口时系统调用此函数取得光眮E
//显示。
HCURSOR CIRHTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIRHTDlg::OnBnClickedButtonOpenfile()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
	CString szFileters("JPEG Files(*.jpg;*.jpeg;*.jpe)|*.jpg;*.jpeg;*.jpe|BMP Files(*.bmp)|*.bmp|All Files (*.*)|*.*||");

	//CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, szFileters, this);

	CFileDialog myDlg(TRUE, _T("*.jpg;*.jpeg;*.jpe"), NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, szFileters, this);

	if(myDlg.DoModal() == IDOK)
	{
		m_inputFilePath = myDlg.GetPathName();  //获得文件 MessageBox(m_edit_inputFilepath);路径名没有皝Eɡ┱姑丒
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
	// TODO: 在此铁赜控件通知处历咸序代聛E
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
		//根据聚类中心，把各个象素点分纴E
		m_img->K_PixelClass();

		ini_timer.Stop();

		//输出图蟻E
		m_img->CreateOutputImg(0);

		cvNamedWindow("Result Of K-Mean", 1);
		cvShowImage("Result Of K-Mean", m_img->m_out_img);

		/* Continue timer */
		ini_timer.Start();

		s_flag = m_img->KMeans();
		++m_img->m_iterativeTimes;

		/* Do not count GUI overhead */
		ini_timer.Stop();

		//输出结箒E
		m_static_segmentationTimes.Format(_T("%d"), m_img->m_iterativeTimes);
		m_static_segmentationTimer.Format(_T("%d ms"), 0);
		UpdateData(false);
	}
	m_static_segmentationTimer.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());
	UpdateData(false);

	//计算方瞾E
	m_img->UpdateVariances();

	//输出图蟻E
	m_img->CreateOutputImg(m_outputType);

	//显示均值觼E讲丒
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
	// TODO: 在此铁赜控件通知处历咸序代聛E
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

	//输出结箒E
	m_static_segmentationTimes.Format(_T("%d"), m_img->m_iterativeTimes);
	m_static_segmentationTimer.Format(_T("%.2f ms"), ini_timer.GetElapsedTimeMs());
	UpdateData(false);

	//计算方瞾E
	m_img->UpdateVariances();

	//输出图蟻E
	m_img->CreateOutputImg(m_outputType);

	//显示均值觼E讲丒
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
	// TODO: 在此铁赜控件通知处历咸序代聛E
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
	// TODO: 在此铁赜控件通知处历咸序代聛E
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
	// TODO: 在此铁赜控件通知处历咸序代聛E
	m_irhtType = 1;
}

void CIRHTDlg::OnBnClickedButtoniirht2()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
	m_irhtType = 2;
}

void CIRHTDlg::OnBnClickedButtonRht()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
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
	// TODO: 在此铁赜控件通知处历咸序代聛E
	cvDestroyAllWindows();
}

void CIRHTDlg::OnBnClickedButtonSnake()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
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
	// TODO:  如果该控件是 RICHEDIT 控件，则藖E换丒
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 眮E尽盎颉痹怂愕窖诼丒小�

	// TODO:  在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);
}

void CIRHTDlg::OnEnChangeEditClassnum()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则藖E换丒
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 眮E尽盎颉痹怂愕窖诼丒小�

	// TODO:  在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);
}

void CIRHTDlg::OnEnChangeEditKirht()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则藖E换丒
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 眮E尽盎颉痹怂愕窖诼丒小�

	// TODO:  在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);
}

void CIRHTDlg::OnCbnSelchangeComboBackgroundtype()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);
}

void CIRHTDlg::OnStnClickedStaticSegmentationtimer()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);
}

void CIRHTDlg::OnBnClickedCheckSkeleton()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);

	if (m_check_skeleton.GetCheck() == 1)
		m_img->SetSkeletonFlag(TRUE);
	else
		m_img->SetSkeletonFlag(FALSE);
}

void CIRHTDlg::OnBnClickedCheckHead()
{
	// TODO: 在此铁赜控件通知处历咸序代聛E
	UpdateData(TRUE);

	if (m_check_headFlag.GetCheck() == 1)
		m_img->SetHeadDectionFlag(TRUE);
	else
		m_img->SetHeadDectionFlag(FALSE);
}


