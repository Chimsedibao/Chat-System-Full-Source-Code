#pragma once
#include <winsock2.h>

enum YeuCauCuaClient
{
    YeuCauDangKy = 1,
    YeuCauDangNhap = 2,
    TinNhanChat = 3,

    PhanHoiDangNhapThanhCong_CoData = 4,
    PhanHoiDangNhapThatBai = 5,

    YeuCauLayDanhSachBan = 6,
    PhanHoiDanhSachBan = 7,

    YeuCauLayLichSuChat = 8,
    PhanHoiLichSuChat = 9,

    YeuCauGuiFile = 10,
    DuLieuFile = 11,
};

struct AuthPacket
{
    wchar_t username[50];
    wchar_t password[50];
};

struct ChatPacket
{
    int IDNhan;
    wchar_t message[512];
};

struct ThongTinClient //me
{
    SOCKET socket;
    wchar_t username[50];
    int UserID;
};

struct BanBeInfo
{
    int UserID;
    wchar_t username[50];
};

struct GoiTinDanhSachBan
{
    int soLuong;
    BanBeInfo danhSach[100];
};

struct GoiTinYeuCauLichSu //OnNMClickListFriend
{
    int IDBanBe;
};

struct LichSuChatPacket
{
    int IDGui;            
    wchar_t Content[512]; 
};

struct GoiTinLichSuChat
{
    int soLuong;
    LichSuChatPacket danhSach[100]; 
};

//sv
struct GoiTinLoginSuccess
{
    int UserID;
    wchar_t username[50];
};

//  file
struct GoiTinFileInfo
{
    int IDNguoiNhan;
    wchar_t tenFile[100];
    long long kichThuoc;
};

//dữ liệu fiel 
struct GoiTinFileData
{
    int IDNguoiNhan;
    int soByte;
    char data[4096];
};

struct ChatHeader
{
    int lenh;        
    int IDNguoiNhan; 
    int doDaiData;   
};