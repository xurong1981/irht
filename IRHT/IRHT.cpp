// IRHT.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "IRHT.h"
#include "IRHTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIRHTApp

BEGIN_MESSAGE_MAP(CIRHTApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CIRHTApp 构詠E

CIRHTApp::CIRHTApp()
{
	// TODO: 在此处铁赜构詠E丒�
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一竵ECIRHTApp 对蟻E

CIRHTApp theApp;


// CIRHTApp 初始化

BOOL CIRHTApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或竵E甙姹纠雌粲每墒踊绞剑�
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将藖E柚梦丒ㄋ幸谟τ贸绦蛑惺褂玫�
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 眮E汲跏蓟�
	// 如果未使用这些功能并希望减小
	// 蛘可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 竵E挠糜诖娲⑸柚玫淖⒉岜丒
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织脕E
	SetRegistryKey(_T("应用程序向导生成的本地应用程�"));

	CIRHTDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处历治时用
		//  “确定”来关闭对话框的代聛E
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处历治时用
		//  “取消”来关闭对话框的代聛E
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
