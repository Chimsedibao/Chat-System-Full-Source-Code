# 🛡️ Secure LAN Chat System

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white)
![SQLite](https://img.shields.io/badge/sqlite-%2307405e.svg?style=for-the-badge&logo=sqlite&logoColor=white)
![Security](https://img.shields.io/badge/Security-AES%20%2F%20SHA256-green?style=for-the-badge)

> Hệ thống Chat LAN toàn diện: Từ giao diện MFC, Server chạy ngầm (Windows Service) đến mã hóa bảo mật cấp cao.

---

## 🚀 Tính năng nổi bật

### 🔒 Bảo mật & An toàn
- [x] **Mã hóa Password:** Sử dụng thuật toán **SHA-256** (Windows CNG) - Chống lộ mật khẩu.
- [x] **Mã hóa Tin nhắn:** Sử dụng **AES-256** để mã hóa nội dung chat - Chống nghe lén (Sniffing).
- [x] **Chống SQL Injection:** Sử dụng Prepared Statements của SQLite.

### ⚙️ Kỹ thuật Hệ thống
- [x] **Windows Service:** Server chạy ngầm (`TU_LANH`), tự khởi động cùng Windows.
- [x] **Multi-threading:** Xử lý đa luồng, đảm bảo hiệu năng cao.
- [x] **Custom Protocol:** Giao thức tự chế (Header + Body) giúp tiết kiệm băng thông.
- [x] **File Transfer:** Gửi file dung lượng lớn bằng kỹ thuật Chunking.

### 🖥️ Giao diện Client
- [x] Đăng ký / Đăng nhập / Logout an toàn.
- [x] Danh sách bạn bè Real-time.
- [x] Lịch sử chat (Lưu trữ vĩnh viễn trên Server).

---

## 🛠️ Cài đặt (Installation)

1. **Clone về máy:** `git clone https://github.com/Ten_Cua_Ban/Ten_Repo.git`
2. **Server:**
   - Copy `ChatServer.exe` và `sqlite3.dll` vào `C:\ServiceChat`.
   - Chạy CMD (Admin): `sc create TU_LANH binPath= "C:\ServiceChat\ChatServer.exe"` sau đó `sc start TU_LANH`.
3. **Client:** Chạy `Client_Khach.exe` và thưởng thức!

---
**Author:** [Tên Của Bạn]
