// IRHT.cpp : ����Ӧ�ó��������Ϊ��
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


// CIRHTApp ��ԁE

CIRHTApp::CIRHTApp()
{
	// TODO: �ڴ˴����ӹ�ԁE�E�
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ��ECIRHTApp ��ρE

CIRHTApp theApp;


// CIRHTApp ��ʼ��

BOOL CIRHTApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ��E߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��ˁE���Ϊ��E�����Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��E���ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// ��E����ڴ洢���õ�ע����E
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯ÁE
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó��"));

	CIRHTDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô�����ʱ��
		//  ��ȷ�������رնԻ���Ĵ�E
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô�����ʱ��
		//  ��ȡ�������رնԻ���Ĵ�E
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
