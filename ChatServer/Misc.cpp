#include "stdafx.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"

void CChatServerDlg::OnBnClickedOk()
{
	PasswordBox->GetWindowTextW(Context.Password, 16);
	Context.ServerThread = AfxBeginThread(CChatServerContext::ServerFunc, &Context);
}

void CChatServerDlg::OnBnClickedButton2()
{
	if (Context.ServerThread)
	{
		if (Context.ServerSocket && Context.ServerSocket != 3435973836 && ~Context.ServerSocket)
			closesocket(Context.ServerSocket);
		WSACancelBlockingCall();
		Context.ServerThread = NULL;
	}
}

BOOL CChatServerContext::Log(WPARAM wParam, CChatClientContext *a) { return CChatServerDlg::Log(Window, wParam, a); }
BOOL CChatServerContext::Log(WPARAM wParam, LPCWSTR a) { return CChatServerDlg::Log(Window, wParam, a); }
BOOL CChatServerContext::Log(WPARAM wParam, LPCWSTR a, PCSTR v) { return CChatServerDlg::Log(Window, wParam, a, v); }
BOOL CChatServerContext::Log(WPARAM wParam, LPCWSTR a, LPCWSTR v) { return CChatServerDlg::Log(Window, wParam, a, v); }
BOOL CChatServerContext::Log(WPARAM wParam, LPCWSTR a, DWORD b) { return CChatServerDlg::Log(Window, wParam, a, b); }
BOOL CChatServerContext::Log(WPARAM wParam, LPCWSTR str, sockaddr_in wtf) { return CChatServerDlg::Log(Window, wParam, str, wtf); }

BOOL CChatClientContext::Log(WPARAM wParam, LPCWSTR a) { return CChatServerDlg::Log(Window, wParam, a); }
BOOL CChatClientContext::Log(WPARAM wParam, LPCWSTR a, PCSTR v) { return CChatServerDlg::Log(Window, wParam, a, v); }
BOOL CChatClientContext::Log(WPARAM wParam, LPCWSTR a, LPCWSTR v) { return CChatServerDlg::Log(Window, wParam, a, v); }
BOOL CChatClientContext::Log(WPARAM wParam, LPCWSTR a, DWORD b) { return CChatServerDlg::Log(Window, wParam, a, b); }
BOOL CChatClientContext::Log(WPARAM wParam, LPCWSTR str, sockaddr_in wtf) { return CChatServerDlg::Log(Window, wParam, str, wtf); }
