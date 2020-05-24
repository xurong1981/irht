// IRHT.cpp : ¶¨ÒåÓ¦ÓÃ³ÌÐòµÄÀàÐÐÎª¡£
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


// CIRHTApp ¹¹ÔE

CIRHTApp::CIRHTApp()
{
	// TODO: ÔÚ´Ë´¦ÌúØÓ¹¹ÔEúÂE¬
	// ½«ËùÓÐÖØÒªµÄ³õÊ¼»¯·ÅÖÃÔÚ InitInstance ÖÐ
}


// Î¨Ò»µÄÒ»¸ECIRHTApp ¶ÔÏE

CIRHTApp theApp;


// CIRHTApp ³õÊ¼»¯

BOOL CIRHTApp::InitInstance()
{
	// Èç¹ûÒ»¸öÔËÐÐÔÚ Windows XP ÉÏµÄÓ¦ÓÃ³ÌÐòÇåµ¥Ö¸¶¨Òª
	// Ê¹ÓÃ ComCtl32.dll °æ±¾ 6 »ò¸Eß°æ±¾À´ÆôÓÃ¿ÉÊÓ»¯·½Ê½£¬
	//ÔòÐèÒª InitCommonControlsEx()¡£·ñÔò£¬½«ÎÞ·¨´´½¨´°¿Ú¡£
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ½«ËEèÖÃÎª°E¨ËùÓÐÒªÔÚÓ¦ÓÃ³ÌÐòÖÐÊ¹ÓÃµÄ
	// ¹«¹²¿Ø¼þÀà¡£
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ±E¼³õÊ¼»¯
	// Èç¹ûÎ´Ê¹ÓÃÕâÐ©¹¦ÄÜ²¢Ï£Íû¼õÐ¡
	// ×ûòÕ¿ÉÖ´ÐÐÎÄ¼þµÄ´óÐ¡£¬ÔòÓ¦ÒÆ³ýÏÂÁÐ
	// ²»ÐèÒªµÄÌØ¶¨³õÊ¼»¯Àý³Ì
	// ¸EÄÓÃÓÚ´æ´¢ÉèÖÃµÄ×¢²á±úëE
	// TODO: Ó¦ÊÊµ±ÐÞ¸Ä¸Ã×Ö·û´®£¬
	// ÀýÈçÐÞ¸ÄÎª¹«Ë¾»ò×éÖ¯ÃE
	SetRegistryKey(_T("Ó¦ÓÃ³ÌÐòÏòµ¼Éú³ÉµÄ±¾µØÓ¦ÓÃ³ÌÐ"));

	CIRHTDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ÔÚ´Ë·ÅÖÃ´¦ÀúÖÎÊ±ÓÃ
		//  ¡°È·¶¨¡±À´¹Ø±Õ¶Ô»°¿òµÄ´úÂE
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ÔÚ´Ë·ÅÖÃ´¦ÀúÖÎÊ±ÓÃ
		//  ¡°È¡Ïû¡±À´¹Ø±Õ¶Ô»°¿òµÄ´úÂE
	}

	// ÓÉÓÚ¶Ô»°¿òÒÑ¹Ø±Õ£¬ËùÒÔ½«·µ»Ø FALSE ÒÔ±ãÍË³öÓ¦ÓÃ³ÌÐò£¬
	//  ¶ø²»ÊÇÆô¶¯Ó¦ÓÃ³ÌÐòµÄÏûÏ¢±Ã¡£
	return FALSE;
}
