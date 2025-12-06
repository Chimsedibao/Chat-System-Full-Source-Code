
// Client_KhachDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Client_Khach.h"
#include "Client_KhachDlg.h"
#include "afxdialogex.h"

#include "Struct.h"
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "CMHChat.h"
#include "CDangKy.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientKhachDlg dialog



CClientKhachDlg::CClientKhachDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_KHACH_DIALOG, pParent)
	, m_user(_T(""))
	, m_pass(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientKhachDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, ID_USER, m_user);
	DDX_Text(pDX, ID_PASS, m_pass);
}

BEGIN_MESSAGE_MAP(CClientKhachDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_DANGNHAP, &CClientKhachDlg::OnBnClickedDangnhap)
	ON_BN_CLICKED(BTN_DK, &CClientKhachDlg::OnBnClickedDk)
END_MESSAGE_MAP()


// CClientKhachDlg message handlers

BOOL CClientKhachDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	// TODO: Add extra initialization here
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	KetNoiLaiServer();  

	return TRUE;  
}

void CClientKhachDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientKhachDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientKhachDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CClientKhachDlg::OnBnClickedDangnhap()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	if (m_user.IsEmpty() || m_pass.IsEmpty()) {
		MessageBox(L"Vui lòng nhập đủ tên và mật khẩu!", L"Thông báo"); return;
	}

	
	int lenh = YeuCauDangNhap;
	send(m_socket, (char*)&lenh, sizeof(int), 0);

	//đẩy packet đi 
	AuthPacket pkt;
	wcscpy_s(pkt.username, m_user);
	wcscpy_s(pkt.password, m_pass);
	send(m_socket, (char*)&pkt, sizeof(AuthPacket), 0);

	
	int ketQua = 0;
	recv(m_socket, (char*)&ketQua, sizeof(int), 0);

	if (ketQua == PhanHoiDangNhapThanhCong_CoData)
	{
		MessageBox(L"Đăng nhập thành công!", L"Thông báo");
		ShowWindow(SW_HIDE);

		CMHChat chatDlg;
		chatDlg.m_socketChat = m_socket; 
		chatDlg.m_myUserName = m_user;   
		chatDlg.DoModal();               

		ShowWindow(SW_SHOW);

		KetNoiLaiServer();

		m_pass = L"";
		UpdateData(FALSE);
		
	}
	else
	{
		MessageBox(L"Sai tài khoản hoặc mật khẩu!", L"Lỗi", MB_ICONERROR);
	}
}

void CClientKhachDlg::OnBnClickedDk()
{
	// TODO: Add your control notification handler code here
	CDangKy dlg;
	dlg.m_socketDK = m_socket;
	dlg.DoModal();
}


	// sửa lỗi  nút logout ... 
void CClientKhachDlg::KetNoiLaiServer()
{

	if (m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9999);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
}
