
// ChatClientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatClientDlg 对话框



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CChatClientDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &CChatClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CChatClientDlg 消息处理程序

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ToBox = (CEdit*)GetDlgItem(IDC_EDIT3);
	LogBox = (CEdit*)GetDlgItem(IDC_EDIT1);
	MsgBox = (CEdit*)GetDlgItem(IDC_EDIT2);
	theApp.hWnd = LogBox;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CChatClientDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CChatClientDlg::OnBnClickedOk()
{
	data_gram thePost;
	memset(thePost.data, 0x00, sizeof(thePost));
	thePost.version = DATAGRAM_EXCHANGE;
	wchar_t buf[30];
	ToBox->GetWindowTextW(buf, 4);
	swscanf_s(buf, L"%d", &thePost.exchange.to);
	MsgBox->GetWindowTextW(thePost.exchange.message, 100);
	theApp.MessageQueue.push(thePost);
	swprintf_s(buf, 30, L"%s: ", theApp.Nickname);
	theApp.Log(5, buf, thePost.exchange.message);
}
