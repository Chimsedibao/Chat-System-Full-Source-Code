// CMHChat.cpp : implementation file
//

#include "pch.h"
#include "Client_Khach.h"
#include "afxdialogex.h"
#include "CMHChat.h"
#include "Struct.h"

// CMHChat dialog

IMPLEMENT_DYNAMIC(CMHChat, CDialogEx)

CMHChat::CMHChat(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MHCHAT, pParent)
	, m_msg(_T(""))
{

}

CMHChat::~CMHChat()
{
}

void CMHChat::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FRIEND, m_listFriend);
	DDX_Control(pDX, IDC_LIST_CHAT, m_listChat);
	DDX_Text(pDX, IDC_EDIT_MSG, m_msg);
}


BEGIN_MESSAGE_MAP(CMHChat, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_SEND, &CMHChat::OnBnClickedBtnSend)
    ON_NOTIFY(NM_CLICK, IDC_LIST_FRIEND, &CMHChat::OnNMClickListFriend)
    ON_BN_CLICKED(IDC_BTN_REFRESH, &CMHChat::OnBnClickedBtnRefresh)
    ON_BN_CLICKED(IDC_BTN_LOGOUT, &CMHChat::OnBnClickedBtnLogout)
    ON_BN_CLICKED(IDC_BTN_FILE, &CMHChat::OnBnClickedBtnFile)
END_MESSAGE_MAP()


BOOL CMHChat::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_idDangChat = -1;

    m_listFriend.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listFriend.InsertColumn(0, L"ID", LVCFMT_LEFT, 40);
    m_listFriend.InsertColumn(1, L"Tên Bạn", LVCFMT_LEFT, 120);

   
    CRect rect; //lớp hình chữ nhật, lưu 4 giá trị Left, Top, Right, Bottom 
    m_listChat.GetClientRect(&rect);

    
    m_listChat.InsertColumn(0, L"Nội dung cuộc trò chuyện", LVCFMT_LEFT, rect.Width());

    int lenh = YeuCauLayDanhSachBan;
    send(m_socketChat, (char*)&lenh, sizeof(int), 0);

    AfxBeginThread(NhanDuLieuThread, this);
    CString strTitle;
    strTitle.Format(L"Chat Client - Xin chào: %s", m_myUserName);
    SetWindowText(strTitle);

    return TRUE;
}


UINT  CMHChat::NhanDuLieuThread(LPVOID pParam)
{
    CMHChat* pThis = (CMHChat*)pParam;
    SOCKET s = pThis->m_socketChat;
    int lenh = 0;

    // biến cho F file làm thêm này
    CFile fWrite;
    long long tongKichThuoc = 0;
    long long daNhan = 0;
    bool dangNhanFile = false;

    
    while (recv(s, (char*)&lenh, sizeof(int), 0) > 0)
    {
        switch (lenh)
        {
        case PhanHoiDanhSachBan:
        {
            GoiTinDanhSachBan goiTin;
            recv(s, (char*)&goiTin, sizeof(GoiTinDanhSachBan), 0);

            pThis->m_listFriend.DeleteAllItems();
            for (int i = 0; i < goiTin.soLuong; i++) {
                CString strID; strID.Format(L"%d", goiTin.danhSach[i].UserID);
                int row = pThis->m_listFriend.InsertItem(i, strID);
                pThis->m_listFriend.SetItemText(row, 1, goiTin.danhSach[i].username);
            }
            break;
        }

        case TinNhanChat:
        {
            
            int idNguoiGui =0;
            int   doDaiData =0;

             recv(s, (char*)&idNguoiGui, sizeof(int), 0);
            recv(s, (char*)&doDaiData, sizeof(int), 0); //size of int hay . 

            // xin ram, cấp phát động z
            char* buffer = new char[doDaiData];

            recv(s, buffer, doDaiData, 0);
         
            if (idNguoiGui == pThis->m_idDangChat)
            {
                wchar_t* noiDung = (wchar_t*)buffer;

                CString tenBan = L"Bạn";
                POSITION pos = pThis->m_listFriend.GetFirstSelectedItemPosition();
                if (pos) {
                    int nItem = pThis->m_listFriend.GetNextSelectedItem(pos);
                    tenBan = pThis->m_listFriend.GetItemText(nItem, 1);
                } 

                // tên : tin nhắn 
                CString strHienThi;
                strHienThi.Format(L"%s : %s", tenBan, noiDung);

                int row = pThis->m_listChat.InsertItem(pThis->m_listChat.GetItemCount(), strHienThi);
                pThis->m_listChat.EnsureVisible(row, FALSE);
            }
            else
            {
                MessageBeep(MB_OK);
            }

            // trả ram 
            delete[] buffer;
            break;
        }

        case PhanHoiLichSuChat: //nâng cấp phân trang .
        {
            GoiTinLichSuChat ls;
            recv(s, (char*)&ls, sizeof(GoiTinLichSuChat), 0);

            pThis->m_listChat.DeleteAllItems();

            int idBanBe = pThis->m_idDangChat;

            for (int i = 0; i < ls.soLuong; i++)
            {
                CString strHienThi;
                if (ls.danhSach[i].IDGui == idBanBe)
                {
                    strHienThi.Format(L"Bạn : %s", ls.danhSach[i].Content);
                }
                else
                {
                    strHienThi.Format(L"Tôi : %s", ls.danhSach[i].Content);
                }

                int row = pThis->m_listChat.InsertItem(pThis->m_listChat.GetItemCount(), strHienThi);
                pThis->m_listChat.EnsureVisible(row, FALSE);
            }
            break;
        }

        case YeuCauGuiFile:
        {
            GoiTinFileInfo info;
            recv(s, (char*)&info, sizeof(GoiTinFileInfo), 0);

            CString tenFileMoi;
            tenFileMoi.Format(L"Download_%s", info.tenFile);

            if (fWrite.Open(tenFileMoi, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
            {
                tongKichThuoc = info.kichThuoc;
                daNhan = 0;
                dangNhanFile = true;

                CString thongBao;
                thongBao.Format(L"Đang nhận file: %s...", info.tenFile);
                pThis->m_listChat.InsertItem(pThis->m_listChat.GetItemCount(), thongBao);
            }
            break;
        }

        case DuLieuFile:
        {
            GoiTinFileData dataPacket;
            recv(s, (char*)&dataPacket, sizeof(GoiTinFileData), 0);

            if (dangNhanFile)
            {
                fWrite.Write(dataPacket.data, dataPacket.soByte);
                daNhan += dataPacket.soByte;

                if (daNhan >= tongKichThuoc)
                {
                    fWrite.Close();
                    dangNhanFile = false;
                    pThis->MessageBox(L"Đã nhận file thành công!", L"Thông báo");
                }
            } 
            break;
        }
         
        default:
            break;
        } 
    } 
    return 0;  
}

void CMHChat::OnBnClickedBtnSend()
{
    UpdateData(TRUE);
    if (m_msg.IsEmpty()) return;

    POSITION pos = m_listFriend.GetFirstSelectedItemPosition();
    if (pos == NULL) return;

    int nItem = m_listFriend.GetNextSelectedItem(pos);
    int idNhan = _ttoi(m_listFriend.GetItemText(nItem, 0));

   
    int doDaiByte = (m_msg.GetLength() + 1) * sizeof(wchar_t);

  
    ChatHeader header;
    header.lenh = TinNhanChat;
    header.IDNguoiNhan = idNhan;
    header.doDaiData = doDaiByte;   

    send(m_socketChat, (char*)&header, sizeof(ChatHeader), 0);

    send(m_socketChat, (char*)m_msg.GetBuffer(), doDaiByte, 0);


    CString strHienThi;
    strHienThi.Format(L"Tôi : %s", m_msg);

    int row = m_listChat.InsertItem(m_listChat.GetItemCount(), strHienThi);
    m_listChat.EnsureVisible(row, FALSE);
    
    m_msg = L"";
    UpdateData(FALSE);
}

void CMHChat::OnNMClickListFriend(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here

    POSITION pos = m_listFriend.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        int nItem = m_listFriend.GetNextSelectedItem(pos);

       
        CString strID = m_listFriend.GetItemText(nItem, 0);
        int idBanBe = _ttoi(strID);  // _ttoi là viết tắt của Text TO Integer(Chuyển Văn bản thành Số nguyên).
        m_idDangChat = idBanBe;

        m_listChat.DeleteAllItems();

        int lenh = YeuCauLayLichSuChat;
        send(m_socketChat, (char*)&lenh, sizeof(int), 0);

        GoiTinYeuCauLichSu pkt;
        pkt.IDBanBe = idBanBe;
        send(m_socketChat, (char*)&pkt, sizeof(GoiTinYeuCauLichSu), 0);
    }
    *pResult = 0;
}

void CMHChat::OnOK()
{
    OnBnClickedBtnSend();
}

void CMHChat::OnBnClickedBtnRefresh()
{
    // TODO: Add your control notification handler code here
    int lenh = YeuCauLayDanhSachBan;
    send(m_socketChat, (char*)&lenh, sizeof(int), 0);
}

void CMHChat::OnBnClickedBtnLogout()
{
    // TODO: Add your control notification handler code here
    EndDialog(IDCANCEL);
}

void CMHChat::OnBnClickedBtnFile()
{
    // TODO: Add your control notification handler code here
    POSITION pos = m_listFriend.GetFirstSelectedItemPosition();
    if (pos == NULL) {
        MessageBox(L"Hãy chọn một người bạn để gửi file!", L"Chưa chọn người");
        return;
    }
    int nItem = m_listFriend.GetNextSelectedItem(pos);
    int idNhan = _ttoi(m_listFriend.GetItemText(nItem, 0));

    CFileDialog dlg(TRUE); 
    if (dlg.DoModal() == IDOK)
    {
        CString duongDan = dlg.GetPathName(); 
        CString tenFile = dlg.GetFileName();  

        CFile file;
        if (!file.Open(duongDan, CFile::modeRead | CFile::typeBinary))
        {
            MessageBox(L"Không mở được file để đọc!", L"Lỗi");
            return;
        }

        long long kichThuocFile = file.GetLength();

        //struct INFO : LỆNH < ID < DATA
        int lenh = YeuCauGuiFile;
        send(m_socketChat, (char*)&lenh, sizeof(int), 0);

        GoiTinFileInfo info;
        info.IDNguoiNhan = idNhan;
        info.kichThuoc = kichThuocFile;
        wcscpy_s(info.tenFile, tenFile);
        send(m_socketChat, (char*)&info, sizeof(GoiTinFileInfo), 0);

        
        char buffer[4096]; // Mỗi lần gửi 4KB
        UINT soByteDocDuoc = 0;

        while ((soByteDocDuoc = file.Read(buffer, 4096)) > 0)
        {
            int lenhData = DuLieuFile;
            send(m_socketChat, (char*)&lenhData, sizeof(int), 0);

            GoiTinFileData dataPacket;
            dataPacket.IDNguoiNhan = idNhan;
            dataPacket.soByte = soByteDocDuoc; 
            memcpy(dataPacket.data, buffer, soByteDocDuoc); 

            send(m_socketChat, (char*)&dataPacket, sizeof(GoiTinFileData), 0);

            Sleep(1);
        }

        file.Close();
        
        CString strThongBao;
        strThongBao.Format(L"Tôi đã gửi file: %s", tenFile);
        int row = m_listChat.InsertItem(m_listChat.GetItemCount(), strThongBao);
        m_listChat.EnsureVisible(row, FALSE);
    }
}
