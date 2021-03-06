#pragma once
#include <string>
#include <vector>
#include "datagram.h"
#include "context.h"
#define WM_UPDATE_STATIC (WM_USER + 100)

class CChatServerDlg : public CDialog
{
public:
	CChatServerDlg(CWnd* pParent = NULL);
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATSERVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);
	CEdit *PasswordBox;
	CListBox *LogBox;
	CListCtrl *UserListBox;
	CButton* BtnOk;
	CButton* BtnStop;
	DECLARE_MESSAGE_MAP()
	CChatServerContext Context;

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2();
	static BOOL Log(HWND hWnd, WPARAM wParam, CChatClientContext *cccc);
	static BOOL Log(HWND hWnd, WPARAM wParam, LPCWSTR a);
	static BOOL Log(HWND hWnd, WPARAM wParam, LPCWSTR a, PCSTR v);
	static BOOL Log(HWND hWnd, WPARAM wParam, LPCWSTR a, LPCWSTR v);
	static BOOL Log(HWND hWnd, WPARAM wParam, LPCWSTR a, DWORD b);
	static BOOL Log(HWND hWnd, WPARAM wParam, LPCWSTR str, sockaddr_in wtf);
};
