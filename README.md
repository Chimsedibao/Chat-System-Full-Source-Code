# 🛡️ Secure LAN Chat System (Windows Service & MFC)

![C++](https://img.shields.io/badge/C++-Solutions-blue.svg?style=for-the-badge&logo=c%2B%2B)
![Windows Service](https://img.shields.io/badge/Windows-Service-0078D6?style=for-the-badge&logo=windows)
![Security](https://img.shields.io/badge/Security-AES%2FSHA256-green?style=for-the-badge)
![Network](https://img.shields.io/badge/Network-TCP%2FIP-orange?style=for-the-badge)

> **Dự án nghiên cứu chuyên sâu về Lập trình Hệ thống (System Programming) và Mạng máy tính trên Windows.**
>  C++ thuần: **Windows Service, Raw Socket, Manual Serialization, Multi-threading và Encryption.**
## 🧠 KNOWLEDGE BASE (Kiến thức Cốt lõi)

đây là những gì bạn sẽ học được:
## 💻 Kiến thức Code & WinAPI (Coding Insights)

Dự án này là bước chuyển đổi tư duy từ **Standard C++** (Console) sang **Windows System Programming**. Bạn sẽ làm quen với các khái niệm đặc thù của Microsoft mà sách giáo khoa cơ bản ít dạy:

### 1. Hệ thống Kiểu dữ liệu Windows (WinAPI Data Types)
Lập trình WinAPI không dùng `int`, `void` thuần túy mà dùng các định nghĩa riêng (Typedef) để đảm bảo tương thích hệ thống:

| Kiểu chuẩn C++ | Kiểu WinAPI | Ý nghĩa & Tại sao dùng? |
| :--- | :--- | :--- |
| `unsigned int` | **`DWORD` / `UINT`** | Kiểu số nguyên dương 32-bit. Dùng trong hầu hết các hàm hệ thống (VD: kích thước file, mã lỗi, cờ trạng thái). |
| `void*` | **`LPVOID`** | Con trỏ vô định (Long Pointer to Void). Được dùng như một "cái hộp đa năng" để truyền dữ liệu qua lại giữa các Thread (Ví dụ: ép kiểu `SOCKET` sang `LPVOID`). |
| `char` | **`TCHAR`** | Kiểu ký tự linh hoạt, tự động đổi thành `char` (ANSI) hoặc `wchar_t` (Unicode) tùy cấu hình Project. |
| `const char*` | **`LPCSTR`** | Con trỏ hằng trỏ đến chuỗi ký tự thường. |
| `const wchar_t*` | **`LPCWSTR`** | Con trỏ hằng trỏ đến chuỗi ký tự rộng (Unicode). |

### 1.2 Xử lý Unicode (wchar_t vs char)
Khác với C++ cơ bản chỉ dùng `std::string` (1 byte/ký tự), dự án này bắt buộc dùng **Unicode (UTF-16)** để hỗ trợ Tiếng Việt có dấu.
* **`wchar_t`**: Ký tự rộng chiếm **2 byte** trong bộ nhớ.
* **Lưu ý quan trọng:** Khi dùng hàm `send()` của Socket (tính theo Byte), bạn phải nhân đôi kích thước: `len = số_ký_tự * sizeof(wchar_t)`.
* **Hàm xử lý chuỗi:** Không dùng `strcpy`, `sprintf` mà phải dùng `wcscpy_s`, `swprintf_s` (các hàm an toàn của Microsoft dành cho Unicode).

### 1.3. Tư duy Quản lý Bộ nhớ (Memory & Pointers)
* **Cấp phát động (Dynamic Allocation):** Không dùng `std::string` hay `std::vector` để gửi qua mạng vì chúng chứa con trỏ ngầm.
* **Kỹ thuật:** Phải tính toán kích thước gói tin (Header), sau đó dùng `new char[size]` để xin vùng nhớ vừa khít, và `delete[]` ngay sau khi xử lý để tránh rò rỉ (Memory Leak).
* **Con trỏ hàm (Function Pointer):** Khi tạo Thread (`CreateThread`), bạn phải hiểu cách truyền một hàm `static` hoặc hàm toàn cục vào làm tham số.

### 1.4. Giao tiếp với Hệ điều hành (System Calls)
Bạn sẽ học cách gọi các API tầng thấp của Windows thay vì thư viện chuẩn:
* `Sleep(ms)`: Tạm dừng luồng.
* `GetLocalTime()`: Lấy giờ hệ thống chi tiết đến mili-giây (thay vì `ctime`).
* `CreateEvent`, `WaitForSingleObject`: Các hàm điều phối luồng cực mạnh của Windows.

### 1.5. Kiến trúc Hệ thống (System Architecture)
* **Windows Service (Native):** Cách chuyển đổi một ứng dụng Console thành **Background Service** chạy ngầm, tự khởi động cùng Windows, quản lý vòng đời qua SCM (Service Control Manager).
* **MFC (Microsoft Foundation Class):** Xây dựng giao diện Desktop cổ điển, xử lý sự kiện bất đồng bộ.

### 2. Kỹ thuật Mạng Nâng cao (Advanced Networking)
* **Custom Protocol (Giao thức tự chế):** Không dùng JSON/XML. Dự án tự định nghĩa gói tin theo mô hình **Header (Fixed) + Body (Dynamic)** để tối ưu băng thông từng byte.
* **TCP/IP Raw Socket:** Xử lý kết nối, gửi/nhận dữ liệu trực tiếp qua `Winsock2`.
* **File Chunking:** Kỹ thuật cắt nhỏ file (4KB/chunk) để gửi các file dung lượng lớn mà không làm nghẽn mạng hay tràn RAM.

### 3. Xử lý Đa luồng & Bộ nhớ (Concurrency & Memory)
* **Multi-threading:** Server sử dụng mô hình **Thread-per-Client**. Mỗi Client kết nối được một luồng riêng phục vụ.
* **Thread Synchronization:** Sử dụng `CRITICAL_SECTION` để khóa (Lock) tài nguyên chung, ngăn chặn xung đột dữ liệu (Race Condition).
* **Memory Management:** Kỹ thuật cấp phát động (`new`/`delete`) dựa trên kích thước gói tin nhận được từ Header.

### 4. Bảo mật & An toàn (Security Implementation)
* **Cryptography Next Generation (CNG):** Sử dụng thư viện `bcrypt.h` chuẩn MSDN.
    * **SHA-256:** Băm mật khẩu một chiều trước khi lưu vào Database.
    * **AES-256:** Mã hóa toàn bộ nội dung tin nhắn trên đường truyền (Chống nghe lén bằng Wireshark).
* **Anti-SQL Injection:** Sử dụng `Prepared Statements` của SQLite thay vì nối chuỗi.

---

## 🛠️ Cấu trúc Dự án (Architecture Overview)

| Thành phần | Công nghệ / Kỹ thuật | Vai trò |
| :--- | :--- | :--- |
| **Server** | **C++ / Windows API** | Chạy dưới dạng Service (`TU_LANH`). Quản lý kết nối, lưu trữ DB, chuyển tiếp tin nhắn/file. |
| **Client** | **MFC / C++** | Giao diện người dùng. Xử lý mã hóa đầu cuối, hiển thị danh sách bạn bè realtime. |
| **Database** | **SQLite 3** | Lưu trữ tài khoản và lịch sử chat vĩnh viễn (File `chat.db`). |
| **Protocol** | **Binary Struct** | Giao thức nhị phân giúp tốc độ truyền tải cực nhanh. |

---

## 📸 Demo Logic

### 1. Quy trình gửi tin nhắn (Secure Flow)
1. **Client A:** Nhập "Hello" -> Mã hóa AES thành `&^%#$@` -> Đóng gói (Header + Body) -> Gửi.
2. **Server:** Nhận Header -> Cấp RAM -> Nhận Body -> Lưu DB (đã giải mã để log) -> Chuyển tiếp `&^%#$@` sang B.
3. **Client B:** Nhận `&^%#$@` -> Giải mã AES -> Hiển thị "Hello".

### 2. Quy trình gửi File (Chunking Flow)
`File Gốc` -> `Cắt 4096 byte` -> `Gói tin Data` -> `Server (Relay)` -> `Client (Ghi nối file)` -> `Lặp lại đến hết`.

---

## 🚀 Hướng dẫn Cài đặt (Deployment)

### Yêu cầu
* Visual Studio 2022 (C++ Desktop).
* Quyền Administrator (để cài Service).

### Bước 1: Server (Chạy ngầm)
1. Build `ChatServer` (Release).
2. Mở CMD (Admin):
   ```cmd
   sc create TU_LANH binPath= "C:\ServiceChat\ChatServer.exe"
   sc start TU_LANH
Bước 2: Client
Chạy Client_Khach.exe.

Đăng ký -> Đăng nhập -> Chat!
