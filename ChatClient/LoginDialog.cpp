#include "stdafx.h"
#include "ChatClient.h"
#include "LoginDialog.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent) {}

CLoginDialog::~CLoginDialog() {}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CLoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	IPBox = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);
	IPBox->SetAddress(127, 0, 0, 1);
	return TRUE;
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDialog::OnBnClickedOk)
END_MESSAGE_MAP()

void CLoginDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItemTextW(IDC_EDIT1, theApp.Password, 16);
	GetDlgItemTextW(IDC_EDIT2, theApp.Nickname, 20);
	IPBox->GetAddress(theApp.TargetIP);
	CDialogEx::OnOK();
}
