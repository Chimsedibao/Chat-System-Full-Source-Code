#pragma once
#include "afxdialogex.h"


// CDangKy dialog

class CDangKy : public CDialogEx
{
	DECLARE_DYNAMIC(CDangKy)

public:
	CDangKy(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDangKy();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DANGKY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_userDK;
	CString m_passDK;
	CString m_passAgainDK;

	SOCKET m_socketDK;
	afx_msg void OnBnClickedOkDk();
};
