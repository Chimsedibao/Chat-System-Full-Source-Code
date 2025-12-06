
// Client_Khach.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CClientKhachApp:
// See Client_Khach.cpp for the implementation of this class
//

class CClientKhachApp : public CWinApp
{
public:
	CClientKhachApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CClientKhachApp theApp;
