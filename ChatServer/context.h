#pragma once
#include "stdafx.h"
#include "datagram.h"

class CChatServerContext;

class CChatClientContext
{
public:
	int ID;
	HWND Window;
	CWinThread *ClientThread;
	SOCKET ClientSocket;
	BOOL Exited;
	sockaddr_in ClientAddr;
	wchar_t Password[17];
	msgqueue MessageQueue;
	wchar_t Nickname[21];
	CChatServerContext *parent;

	BOOL Log(WPARAM wParam, LPCWSTR a);
	BOOL Log(WPARAM wParam, LPCWSTR a, PCSTR v);
	BOOL Log(WPARAM wParam, LPCWSTR a, LPCWSTR v);
	BOOL Log(WPARAM wParam, LPCWSTR a, DWORD b);
	BOOL Log(WPARAM wParam, LPCWSTR str, sockaddr_in wtf);
	CChatClientContext(LPVOID, SOCKET, const sockaddr_in &);
};

class CChatServerContext
{
public:
	HWND Window;
	CWinThread *ServerThread;
	SOCKET ServerSocket;
	std::vector<CChatClientContext*> ThreadPool;
	wchar_t Password[17];

	void ActivateClient(SOCKET sock, sockaddr_in in);
	static UINT ServerFunc(LPVOID);
	static UINT ClientFunc(LPVOID);
	BOOL Log(WPARAM wParam, CChatClientContext *a);
	BOOL Log(WPARAM wParam, LPCWSTR a);
	BOOL Log(WPARAM wParam, LPCWSTR a, PCSTR v);
	BOOL Log(WPARAM wParam, LPCWSTR a, LPCWSTR v);
	BOOL Log(WPARAM wParam, LPCWSTR a, DWORD b);
	BOOL Log(WPARAM wParam, LPCWSTR str, sockaddr_in wtf);
};
