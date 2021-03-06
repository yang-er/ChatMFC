#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "LoginDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CChatClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CChatClientApp::CChatClientApp()
{
	ClientSocket = NULL;
}

CChatClientApp theApp;

BOOL CChatClientApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CLoginDialog logindlg;
	INT_PTR nResponse = logindlg.DoModal();
	if (nResponse == IDOK)
	{
		Thread = AfxBeginThread(ClientThread, this);

		CChatClientDlg dlg;
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: 在此放置处理何时用
			//  “确定”来关闭对话框的代码
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: 在此放置处理何时用
			//  “取消”来关闭对话框的代码
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
			TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
		}
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CChatClientApp::Log(WPARAM wParam, LPCWSTR str)
{
	if (hWnd == NULL)
		return MessageBoxW(NULL, str, L"Message", MB_OK);
	int len = wcslen(str) + 3;
	LPWSTR logs = new wchar_t[len];
	wcscpy_s(logs, len, str);
	logs[len - 3] = L'\r';
	logs[len - 2] = L'\n';
	logs[len - 1] = L'\0';

	hWnd->SetRedraw(FALSE);
	int nLength = hWnd->GetWindowTextLengthW();
	hWnd->SetSel(nLength, nLength);
	hWnd->ReplaceSel(logs);
	hWnd->LineScroll(((CEdit*)hWnd)->GetLineCount());
	hWnd->SetRedraw(TRUE);

	OutputDebugStringW(logs);
	return TRUE;
}

BOOL CChatClientApp::Log(WPARAM wParam, LPCWSTR str, PCSTR wtf)
{
	wchar_t buffer[300];
	swprintf(buffer, 300, L"%s%hs", str, wtf);
	return Log(wParam, buffer);
}

BOOL CChatClientApp::Log(WPARAM wParam, LPCWSTR str, LPCWSTR wtf)
{
	wchar_t buffer[300];
	swprintf(buffer, 300, L"%s%s", str, wtf);
	return Log(wParam, buffer);
}

BOOL CChatClientApp::Log(WPARAM wParam, LPCWSTR str, sockaddr_in wtf)
{
	wchar_t buffer[300];
	swprintf(buffer, 300, L"%s%hs:%hu", str, inet_ntoa(wtf.sin_addr), wtf.sin_port);
	return Log(wParam, buffer);
}

BOOL CChatClientApp::Log(WPARAM wParam, LPCWSTR str, DWORD data)
{
	LPVOID lpMsgBuf;
	wchar_t buffer[300];
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, data, 0x0409, (LPWSTR)&lpMsgBuf, 0, NULL);
	swprintf(buffer, 300, L"%s :: 0x%08x - %s", str, data, (LPWSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return Log(wParam, buffer);
}
