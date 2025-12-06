#pragma once
#include "afxdialogex.h"


// CMHChat dialog

class CMHChat : public CDialogEx
{
	DECLARE_DYNAMIC(CMHChat)

public:
	CMHChat(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMHChat();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MHCHAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listFriend;
	CListCtrl m_listChat;
	CString m_msg;

	SOCKET m_socketChat;
	CString m_myUserName;
	int m_idDangChat; 

	static UINT  NhanDuLieuThread(LPVOID pParam); 
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnNMClickListFriend(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedBtnLogout();
	afx_msg void OnBnClickedBtnFile();
};
