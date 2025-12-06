// CDangKy.cpp : implementation file
//

#include "pch.h"
#include "Client_Khach.h"
#include "afxdialogex.h"
#include "CDangKy.h"
#include "Struct.h"

// CDangKy dialog

IMPLEMENT_DYNAMIC(CDangKy, CDialogEx)

CDangKy::CDangKy(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DANGKY, pParent)
	, m_userDK(_T(""))
	, m_passDK(_T(""))
	, m_passAgainDK(_T(""))
{

}

CDangKy::~CDangKy()
{
}

void CDangKy::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, ID_USER_DK, m_userDK);
	DDX_Text(pDX, ID_PASS_DK, m_passDK);
	DDX_Text(pDX, ID_PASS_AGAIN_DK, m_passAgainDK);
}


BEGIN_MESSAGE_MAP(CDangKy, CDialogEx)
	ON_BN_CLICKED(BTN_OK_DK, &CDangKy::OnBnClickedOkDk)
END_MESSAGE_MAP()



void CDangKy::OnBnClickedOkDk()
{
    UpdateData(TRUE);
    if (m_userDK.IsEmpty() || m_passDK.IsEmpty()) {
        MessageBox(L"Không được để trống!", L"Lỗi"); return;
    }
    if (m_passDK != m_passAgainDK) {
        MessageBox(L"Mật khẩu nhập lại không khớp!", L"Lỗi"); return;
    }

    int lenh = YeuCauDangKy;
    send(m_socketDK, (char*)&lenh, sizeof(int), 0);

    AuthPacket pkt;
    wcscpy_s(pkt.username, m_userDK);
    wcscpy_s(pkt.password, m_passDK);
    send(m_socketDK, (char*)&pkt, sizeof(AuthPacket), 0);

    MessageBox(L"Đã gửi yêu cầu đăng ký! Hãy thử đăng nhập.", L"Thông báo");
    OnOK(); 
}
