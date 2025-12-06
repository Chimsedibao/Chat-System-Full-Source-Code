#include <winsock2.h> // bọn mạng win và ws2 để đầu tiên
#include <ws2tcpip.h>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include <stdio.h>

#include "sqlite3.h"
#include "Struct.h"
#include <bcrypt.h> 

#pragma comment(lib, "ws2_32.lib")    
#pragma comment(lib, "advapi32.lib")  
#pragma comment(lib, "bcrypt.lib")    

using namespace std;

#define SVCNAME TEXT("TU_LANH")

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

SOCKET g_ListenSocket = INVALID_SOCKET;
vector<ThongTinClient> ds;
CRITICAL_SECTION khoa;
sqlite3* db;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR*);
VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);
VOID SvcReportEvent(LPTSTR);
DWORD WINAPI ServerWorkerThread(LPVOID lpParam);

void GhiLog(const wchar_t* format, ...)
{
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, 1024, format, args);
    va_end(args);

    FILE* f = NULL;
    _wfopen_s(&f, L"C:\\database\\log_tulanh.txt", L"a");
    if (f != NULL ) {
        SYSTEMTIME lt; 
        GetLocalTime(&lt);
        fwprintf(f, L"[%02d:%02d:%02d] %s\n", lt.wHour, lt.wMinute, lt.wSecond, buffer);
        fclose(f);
    }
}

// Hàm Mã Hóa SHA-256 (CNG)
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

void MaHoaSHA256_CNG(const wchar_t* input, wchar_t* outputHex)
{
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    DWORD cbHash = 0, cbData = 0, cbHashObject = 0;
    PBYTE pbHashObject = NULL;
    PBYTE pbHash = NULL;

    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) return;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0);

    pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
    pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);

    if (NT_SUCCESS(BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0))) {
        if (NT_SUCCESS(BCryptHashData(hHash, (PBYTE)input, (ULONG)(wcslen(input) * sizeof(wchar_t)), 0))) {
            if (NT_SUCCESS(BCryptFinishHash(hHash, pbHash, cbHash, 0))) {
                for (DWORD i = 0; i < cbHash; i++) swprintf_s(outputHex + (i * 2), 3, L"%02X", pbHash[i]);
                outputHex[cbHash * 2] = 0;
            }
        }
    }
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    if (hHash) BCryptDestroyHash(hHash);
    if (pbHashObject) HeapFree(GetProcessHeap(), 0, pbHashObject);
    if (pbHash) HeapFree(GetProcessHeap(), 0, pbHash);
}


bool MoDatabase()
{
    int ketQua = sqlite3_open16(L"C:\\database\\chat.db", &db);
    if (ketQua != SQLITE_OK) {
        GhiLog(L"LOI: Khong mo duoc Database tai C:\\database\\chat.db");
        return false;
    }

    sqlite3_stmt* stmt;
    const wchar_t* sqlAcc = L"CREATE TABLE IF NOT EXISTS Account (UserID INTEGER PRIMARY KEY AUTOINCREMENT, Username TEXT NOT NULL UNIQUE, Password TEXT NOT NULL);";
    sqlite3_prepare16_v2(db, sqlAcc, -1, &stmt, 0);
    sqlite3_step(stmt); sqlite3_finalize(stmt);

    const wchar_t* sqlMsg = L"CREATE TABLE IF NOT EXISTS TinNhan (TinNhanID INTEGER PRIMARY KEY AUTOINCREMENT, IDGui INTEGER NOT NULL, IDNhan INTEGER NOT NULL, NoiDung TEXT NOT NULL);";
    sqlite3_prepare16_v2(db, sqlMsg, -1, &stmt, 0); 
    sqlite3_step(stmt); sqlite3_finalize(stmt);

    return true;
}

void XuLyDangKy(wchar_t* ten, wchar_t* matkhau)
{
    wchar_t hash[65]; 
    MaHoaSHA256_CNG(matkhau, hash);
    const wchar_t* sql = L"INSERT INTO Account (Username, Password) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare16_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text16(stmt, 1, ten, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, 2, hash, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

int XuLyDangNhap(wchar_t* ten, wchar_t* matkhau)
{
    wchar_t hash[65]; 
    MaHoaSHA256_CNG(matkhau, hash);
    const wchar_t* sql = L"SELECT UserID FROM Account WHERE Username=? AND Password=?;";
    sqlite3_stmt* stmt;
    int UserID = 0;
    if (sqlite3_prepare16_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, ten, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, 2, hash, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) UserID = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return UserID; 
} 
 
void LuuTinNhanVaoDB(int IDGui, int IDNhan, wchar_t* noiDung)
{
    const wchar_t* sql = L"INSERT INTO TinNhan (IDGui, IDNhan, NoiDung) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare16_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, IDGui);
        sqlite3_bind_int(stmt, 2, IDNhan);
        sqlite3_bind_text16(stmt, 3, noiDung, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

SOCKET TimSocketBangID(int UserID)
{
    SOCKET ketQuaSocket = INVALID_SOCKET; 
    EnterCriticalSection(&khoa);
    for (int i = 0; i < ds.size(); i++)
    {
        if (ds[i].UserID == UserID)
        {
            ketQuaSocket = ds[i].socket;
            break;
        }
    }
    LeaveCriticalSection(&khoa);
    return ketQuaSocket;
}

void GuiDanhSachBan(SOCKET s, int id)
{
    const wchar_t* sql = L"SELECT UserID, Username FROM Account WHERE UserID != ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare16_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return;
    sqlite3_bind_int(stmt, 1, id);

    GoiTinDanhSachBan p; 
    p.soLuong = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        p.danhSach[p.soLuong].UserID = sqlite3_column_int(stmt, 0);
        wcscpy_s(p.danhSach[p.soLuong].username, (const wchar_t*)sqlite3_column_text16(stmt, 1));
        p.soLuong++; if (p.soLuong >= 100) break; 
    }
    sqlite3_finalize(stmt);

    int lenh = PhanHoiDanhSachBan;  
    send(s, (char*)&lenh, sizeof(int), 0);    
    send(s, (char*)&p, sizeof(GoiTinDanhSachBan), 0); 
} 
  
void GuiLichSuChat(SOCKET s, int myID, int friendID)
{
    const wchar_t* sql = L"SELECT IDGui, NoiDung FROM TinNhan WHERE (IDGui=? AND IDNhan=?) OR (IDGui=? AND IDNhan=?) ORDER BY TinNhanID ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare16_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return;
    sqlite3_bind_int(stmt, 1, myID); 
    sqlite3_bind_int(stmt, 2, friendID);
    sqlite3_bind_int(stmt, 3, friendID); 
    sqlite3_bind_int(stmt, 4, myID);

    GoiTinLichSuChat p; p.soLuong = 0;   
    while (sqlite3_step(stmt) == SQLITE_ROW) {      
        p.danhSach[p.soLuong].IDGui = sqlite3_column_int(stmt, 0);  
        wcscpy_s(p.danhSach[p.soLuong].Content, (const wchar_t*)sqlite3_column_text16(stmt, 1));
        p.soLuong++; if (p.soLuong >= 100) break;   
    }   
    sqlite3_finalize(stmt); 
        
    int lenh = PhanHoiLichSuChat;
    send(s, (char*)&lenh, sizeof(int), 0);
    send(s, (char*)&p, sizeof(GoiTinLichSuChat), 0);
}

DWORD WINAPI HandleClient(LPVOID lpParam)
{
    SOCKET ClientSocket = (SOCKET)lpParam;
    int iResult;
    ThongTinClient me = { 0 };
    me.socket = ClientSocket;
    bool logged = false;

    do {
        int lenh = 0;
        iResult = recv(ClientSocket, (char*)&lenh, sizeof(int), 0);
        if (iResult <= 0) break; 

        switch (lenh) {  
        case YeuCauDangKy: 
        {   
            AuthPacket p;  
            recv(ClientSocket, (char*)&p, sizeof(p), 0);  
            XuLyDangKy(p.username, p.password);  
            break;  
        } 
        case YeuCauDangNhap: 
        {   
            AuthPacket packet;  
            recv(ClientSocket, (char*)&packet, sizeof(packet), 0);
            int id = XuLyDangNhap(packet.username, packet.password);
            if (id > 0) {
                me.UserID = id;
          wcscpy_s(me.username, packet.username); 
                logged = true;
                EnterCriticalSection(&khoa); 
                ds.push_back(me); 
                LeaveCriticalSection(&khoa);

                GoiTinLoginSuccess rp; 
                rp.UserID = id; 
                wcscpy_s(rp.username, packet.username);  
int rLenh =  PhanHoiDangNhapThanhCong_CoData;  
                send(ClientSocket, (char*)&rLenh, 4, 0);
                send(ClientSocket, (char*)&rp, sizeof(rp), 0);
            }
            else {
                int rLenh = PhanHoiDangNhapThatBai;
                send(ClientSocket, (char*)&rLenh, 4, 0);
            }
            break;
        }
        case TinNhanChat: {
           
            int idNguoiNhan = 0, doDai = 0;  
            recv(ClientSocket, (char*)&idNguoiNhan, 4, 0);  
            recv(ClientSocket, (char*)&doDai, 4, 0);  

            if (doDai <= 0 || doDai > 10000000) 
                break; 

            char* buf = new char[doDai];
            recv(ClientSocket, buf, doDai, 0);

            if (logged) {
                LuuTinNhanVaoDB(me.UserID, idNguoiNhan, (wchar_t*)buf);
                SOCKET sNhan = TimSocketBangID(idNguoiNhan);
                if (sNhan != INVALID_SOCKET) {
                    ChatHeader header = { TinNhanChat, me.UserID, doDai }; 
                    send(sNhan, (char*)&header.lenh, 4, 0); // Lệnh
                    send(sNhan, (char*)&header.IDNguoiNhan, 4, 0); // ID
                    send(sNhan, (char*)&header.doDaiData, 4, 0); // Dài
                    send(sNhan, buf, doDai, 0);
                }
            }
            delete[] buf;
            break;
        }
        case YeuCauLayDanhSachBan: if (logged) GuiDanhSachBan(ClientSocket, me.UserID);
            break;
        case YeuCauLayLichSuChat: {
            if (logged) {
                GoiTinYeuCauLichSu p;
                recv(ClientSocket, (char*)&p, sizeof(p), 0);
                GuiLichSuChat(ClientSocket, me.UserID, p.IDBanBe);
            }
            break;
        }
        case YeuCauGuiFile: {
            GoiTinFileInfo p; recv(ClientSocket, (char*)&p, sizeof(p), 0);
            SOCKET sNhan = TimSocketBangID(p.IDNguoiNhan);
            if (sNhan != INVALID_SOCKET) {
                int lenh = YeuCauGuiFile;
                send(sNhan, (char*)&lenh, 4, 0);
                p.IDNguoiNhan = me.UserID; //cho ng nhận
                send(sNhan, (char*)&p, sizeof(p), 0);
            }
            break;
        }
        case DuLieuFile: {
            GoiTinFileData p; 
            recv(ClientSocket, (char*)&p, sizeof(p), 0);
            SOCKET sNhan = TimSocketBangID(p.IDNguoiNhan);
            if (sNhan != INVALID_SOCKET) {
                int l = DuLieuFile;
                send(sNhan, (char*)&l, 4, 0);
                send(sNhan, (char*)&p, sizeof(p), 0);
            }
            break;
        }
        }
    } while (iResult > 0);

    if (logged) {
        EnterCriticalSection(&khoa);
        for (int i = 0; i < ds.size(); i++) {
            if (ds[i].socket == ClientSocket) { ds.erase(ds.begin() + i); break; }
        }
        LeaveCriticalSection(&khoa);
    }
    closesocket(ClientSocket);
    return 0;
}


void __cdecl _tmain(int argc, TCHAR* argv[])
{
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPWSTR)SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
        { NULL, NULL }
    };


    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        
        GhiLog(L"Loi StartServiceCtrlDispatcher (%d)", GetLastError());
    }
}



VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    gSvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);

    if (!gSvcStatusHandle) {
        GhiLog(L"Loi RegisterServiceCtrlHandler");
        return;
    }

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    SvcInit(dwArgc, lpszArgv);
}


VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
    ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (ghSvcStopEvent == NULL) {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    GhiLog(L"Dich vu TU_LANH da khoi dong. Bat dau Server...");



    HANDLE hThread = CreateThread(NULL, 0, ServerWorkerThread, NULL, 0, NULL); //

    WaitForSingleObject(ghSvcStopEvent, INFINITE);

    if (hThread) WaitForSingleObject(hThread, 5000);

    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}


VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        if (g_ListenSocket != INVALID_SOCKET) {
            closesocket(g_ListenSocket);
            g_ListenSocket = INVALID_SOCKET;
        }

        SetEvent(ghSvcStopEvent);
        ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }
}


VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else
        gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else
        gSvcStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}


DWORD WINAPI ServerWorkerThread(LPVOID lpParam)
{
    InitializeCriticalSection(&khoa);
    if (!MoDatabase()) { GhiLog(L"Loi mo DB"); return 1; }

    WSADATA wsa; WSAStartup(0x0202, &wsa);
    g_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9999);

    if ( bind(g_ListenSocket, (SOCKADDR*)&addr,sizeof(addr))== SOCKET_ERROR) {
        GhiLog(L"Loi Bind cong 9999");
        return 1; //false
    }
    listen(g_ListenSocket, SOMAXCONN);
    GhiLog(L"Server dang lang nghe...");

    SOCKET client;
    while (WaitForSingleObject(ghSvcStopEvent, 0) != WAIT_OBJECT_0) {
        client = accept(g_ListenSocket, NULL, NULL);
        if (client != INVALID_SOCKET) {
            GhiLog(L"Co khach ket noi");
            CreateThread(NULL, 0, HandleClient, (LPVOID)client, 0, NULL);
        }
    }

    sqlite3_close(db);
    DeleteCriticalSection(&khoa);
    closesocket(g_ListenSocket);
    WSACleanup();
    return 0;
}