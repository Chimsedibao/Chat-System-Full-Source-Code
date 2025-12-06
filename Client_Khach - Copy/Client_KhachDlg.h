
// Client_KhachDlg.h : header file
//

#pragma once
#include "Struct.h"

// CClientKhachDlg dialog
class CClientKhachDlg : public CDialogEx
{
// Construction
public:
	CClientKhachDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_KHACH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:


	CString m_user;
	CString m_pass;

	SOCKET m_socket;
	afx_msg void OnBnClickedDangnhap();
	afx_msg void OnBnClickedDk();
	void KetNoiLaiServer();
};
