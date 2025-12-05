# 📖 HƯỚNG DẪN CHI TIẾT CHẠY DỰ ÁN SMART TRASH CLASSIFICATION

## 📋 MỤC LỤC
1. [Yêu cầu hệ thống](#1-yêu-cầu-hệ-thống)
2. [Cài đặt Python](#2-cài-đặt-python)
3. [Cài đặt thư viện](#3-cài-đặt-thư-viện)
4. [Lấy OpenAI API Key](#4-lấy-openai-api-key)
5. [Cấu hình API Key](#5-cấu-hình-api-key)
6. [Chạy Server](#6-chạy-server)
7. [Truy cập Web Dashboard](#7-truy-cập-web-dashboard)
8. [Kiểm tra hoạt động](#8-kiểm-tra-hoạt-động)
9. [Xử lý lỗi thường gặp](#9-xử-lý-lỗi-thường-gặp)
10. [Cấu trúc thư mục](#10-cấu-trúc-thư-mục)

---

## 1. YÊU CẦU HỆ THỐNG

### Hệ điều hành:
- ✅ Windows 10/11
- ✅ macOS
- ✅ Linux (Ubuntu, Debian, etc.)

### Phần mềm cần thiết:
- Python 3.8 trở lên
- Trình duyệt web (Chrome, Firefox, Edge, Safari)
- Kết nối Internet (để sử dụng OpenAI API)

---

## 2. CÀI ĐẶT PYTHON

### Bước 1: Kiểm tra Python đã cài đặt chưa

Mở **Command Prompt** (Windows) hoặc **Terminal** (macOS/Linux) và gõ:

```bash
python --version
```

Hoặc:

```bash
python3 --version
```

**Kết quả mong đợi:** `Python 3.8.x` hoặc cao hơn

### Bước 2: Nếu chưa có Python, cài đặt như sau:

#### Windows:
1. Truy cập: https://www.python.org/downloads/
2. Tải Python 3.11 hoặc mới hơn
3. Chạy file installer
4. ⚠️ **QUAN TRỌNG:** Tích chọn "Add Python to PATH"
5. Click "Install Now"
6. Đợi cài đặt hoàn tất

#### macOS:
```bash
# Sử dụng Homebrew (nếu đã cài)
brew install python3

# Hoặc tải từ website
# https://www.python.org/downloads/
```

#### Linux (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install python3 python3-pip
```

### Bước 3: Xác nhận cài đặt thành công

Mở lại Command Prompt/Terminal và kiểm tra:

```bash
python --version
pip --version
```

---

## 3. CÀI ĐẶT THƯ VIỆN

### Bước 1: Mở Command Prompt/Terminal

- **Windows:** Nhấn `Win + R`, gõ `cmd`, nhấn Enter
- **macOS/Linux:** Mở Terminal

### Bước 2: Di chuyển đến thư mục dự án

```bash
# Thay đổi đường dẫn theo vị trí dự án của bạn
cd D:\trash_classication_ai
```

**Lưu ý:** Nếu dự án ở ổ khác, ví dụ `C:\Users\YourName\Documents\trash_classication_ai`, thì:

```bash
cd C:\Users\YourName\Documents\trash_classication_ai
```

### Bước 3: Cài đặt các thư viện cần thiết

Chạy lệnh sau:

```bash
pip install flask openai pillow
```

**Hoặc nếu dùng python3:**

```bash
pip3 install flask openai pillow
```

### Bước 4: Xác nhận cài đặt thành công

Kiểm tra các thư viện đã được cài:

```bash
pip list | findstr flask
pip list | findstr openai
pip list | findstr Pillow
```

**Kết quả mong đợi:** Hiển thị tên và phiên bản của các thư viện

---

## 4. LẤY OPENAI API KEY

### Bước 1: Đăng ký/Đăng nhập tài khoản OpenAI

1. Truy cập: https://platform.openai.com/
2. Đăng ký tài khoản mới hoặc đăng nhập nếu đã có

### Bước 2: Tạo API Key

1. Sau khi đăng nhập, click vào **Profile** (góc phải trên)
2. Chọn **View API keys**
3. Click **Create new secret key**
4. Đặt tên cho key (ví dụ: "Smart Trash Project")
5. **SAO CHÉP KEY NGAY** - bạn sẽ không thể xem lại key này sau khi đóng cửa sổ
6. Lưu key vào nơi an toàn

**⚠️ QUAN TRỌNG:** 
- Không chia sẻ API key với ai
- Không commit key lên GitHub
- Key có dạng: `sk-proj-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx`

### Bước 3: Kiểm tra số dư tài khoản

1. Vào **Billing** > **Usage**
2. Đảm bảo tài khoản có credit (OpenAI có thể cho credit miễn phí khi đăng ký)

---

## 5. CẤU HÌNH API KEY

### Bước 1: Mở file cấu hình

Mở file: `file_code/smart_bin_server_openai.py`

Bạn có thể:
- Mở bằng Notepad, VS Code, hoặc bất kỳ trình soạn thảo nào
- Hoặc dùng lệnh trong Command Prompt:

```bash
notepad file_code\smart_bin_server_openai.py
```

### Bước 2: Tìm dòng cấu hình API Key

Tìm dòng số 11 (hoặc tìm text `YOUR_OPENAI_API_KEY_HERE`):

```python
client = OpenAI(api_key="YOUR_OPENAI_API_KEY_HERE")
```

### Bước 3: Thay thế API Key

Thay `YOUR_OPENAI_API_KEY_HERE` bằng API key thật của bạn:

```python
client = OpenAI(api_key="sk-proj-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
```

**Ví dụ:**

```python
client = OpenAI(api_key="sk-proj-abc123def456ghi789jkl012mno345pqr678stu901vwx234")
```

### Bước 4: Lưu file

- Nhấn `Ctrl + S` để lưu
- Đảm bảo file đã được lưu thành công

---

## 6. CHẠY SERVER

### Bước 1: Mở Command Prompt/Terminal

Đảm bảo bạn đang ở thư mục gốc của dự án:

```bash
cd D:\trash_classication_ai
```

### Bước 2: Di chuyển vào thư mục file_code

```bash
cd file_code
```

### Bước 3: Chạy server Flask

```bash
python smart_bin_server_openai.py
```

**Hoặc nếu dùng python3:**

```bash
python3 smart_bin_server_openai.py
```

### Bước 4: Kiểm tra server đã chạy

Bạn sẽ thấy thông báo tương tự:

```
==================================================
SMART BIN AI SERVER (OpenAI)
==================================================
Server chạy tại: http://localhost:5000/
API endpoints:
  - GET  /              -> Dashboard web
  - POST /classify      -> API cho ESP32
  - POST /classify-web  -> API cho web upload
  - GET  /api/statistics -> Thống kê
  - GET  /api/history   -> Lịch sử
==================================================
 * Serving Flask app 'smart_bin_server_openai'
 * Debug mode: on
 * Running on http://0.0.0.0:5000
```

**✅ Nếu thấy thông báo trên = Server đã chạy thành công!**

### Bước 5: Giữ cửa sổ Command Prompt/Terminal mở

⚠️ **KHÔNG ĐÓNG** cửa sổ này. Server cần chạy liên tục để web hoạt động.

Nếu muốn dừng server, nhấn `Ctrl + C`

---

## 7. TRUY CẬP WEB DASHBOARD

### Bước 1: Mở trình duyệt web

Mở bất kỳ trình duyệt nào:
- Google Chrome (khuyên dùng)
- Microsoft Edge
- Firefox
- Safari

### Bước 2: Truy cập địa chỉ

Gõ vào thanh địa chỉ:

```
http://localhost:5000/
```

Hoặc:

```
http://127.0.0.1:5000/
```

### Bước 3: Xem Dashboard

Bạn sẽ thấy:
- **Header:** "Smart Trash Classification AI"
- **Thẻ thống kê:** 4 thẻ hiển thị số lượng RECYCLE, ORGANIC, OTHER, TỔNG CỘNG
- **Biểu đồ:** Biểu đồ đường phân loại theo thời gian
- **Lịch sử:** Danh sách các lần phân loại gần đây

**Nếu thấy trang web = Thành công! 🎉**

---

## 8. KIỂM TRA HOẠT ĐỘNG

### Kiểm tra 1: Server đang chạy

- Command Prompt/Terminal hiển thị thông báo server
- Không có thông báo lỗi màu đỏ

### Kiểm tra 2: Web hiển thị đúng

- Trang web load được
- Thấy các thẻ thống kê (có thể hiển thị 0 nếu chưa có dữ liệu)
- Thấy biểu đồ (có thể trống nếu chưa có dữ liệu)
- Thấy phần lịch sử

### Kiểm tra 3: API hoạt động

Mở tab mới trong trình duyệt và truy cập:

```
http://localhost:5000/api/statistics
```

**Kết quả mong đợi:** Hiển thị JSON như:

```json
{
  "recycle": 0,
  "organic": 0,
  "other": 0,
  "total": 0,
  "timeSeries": [...]
}
```

### Kiểm tra 4: Lịch sử API

Truy cập:

```
http://localhost:5000/api/history
```

**Kết quả mong đợi:** Hiển thị mảng JSON (có thể rỗng `[]` nếu chưa có dữ liệu)

---

## 9. XỬ LÝ LỖI THƯỜNG GẶP

### ❌ Lỗi 1: "python is not recognized"

**Nguyên nhân:** Python chưa được thêm vào PATH

**Giải pháp:**
1. Gỡ cài đặt Python
2. Cài lại Python và **TÍCH CHỌN** "Add Python to PATH"
3. Hoặc thêm Python vào PATH thủ công

### ❌ Lỗi 2: "No module named 'flask'"

**Nguyên nhân:** Chưa cài đặt thư viện

**Giải pháp:**
```bash
pip install flask openai pillow
```

Nếu vẫn lỗi, thử:
```bash
python -m pip install flask openai pillow
```

### ❌ Lỗi 3: "Address already in use" hoặc "Port 5000 is already in use"

**Nguyên nhân:** Port 5000 đang được sử dụng bởi ứng dụng khác

**Giải pháp 1:** Tìm và đóng ứng dụng đang dùng port 5000

**Giải pháp 2:** Đổi port trong file `smart_bin_server_openai.py`:

Tìm dòng cuối cùng:
```python
app.run(host="0.0.0.0", port=5000, debug=True)
```

Đổi thành:
```python
app.run(host="0.0.0.0", port=5001, debug=True)
```

Sau đó truy cập: `http://localhost:5001/`

### ❌ Lỗi 4: "Invalid API Key" hoặc "Incorrect API key provided"

**Nguyên nhân:** API key không đúng hoặc chưa được cấu hình

**Giải pháp:**
1. Kiểm tra lại API key trong file `smart_bin_server_openai.py`
2. Đảm bảo API key được đặt trong dấu ngoặc kép: `"sk-proj-..."`
3. Kiểm tra API key trên OpenAI website còn hoạt động không
4. Đảm bảo tài khoản có credit

### ❌ Lỗi 5: "Cannot GET /" hoặc "404 Not Found"

**Nguyên nhân:** Đường dẫn file không đúng

**Giải pháp:**
1. Kiểm tra file `web/index.html` có tồn tại không
2. Đảm bảo cấu trúc thư mục đúng:
   ```
   trash_classication_ai/
   ├── file_code/
   │   └── smart_bin_server_openai.py
   └── web/
       ├── index.html
       ├── styles.css
       └── main.js
   ```
3. Kiểm tra trong file `smart_bin_server_openai.py` có dòng:
   ```python
   app = Flask(__name__, static_folder='../web', static_url_path='')
   ```

### ❌ Lỗi 6: Web hiển thị nhưng không có dữ liệu

**Nguyên nhân:** Chưa có dữ liệu phân loại

**Giải pháp:**
- Đây là bình thường nếu chưa có lần phân loại nào
- Dữ liệu sẽ xuất hiện sau khi ESP32 gửi ảnh lên server
- Hoặc bạn có thể test bằng cách gọi API `/classify` với ảnh

### ❌ Lỗi 7: "Connection refused" khi truy cập web

**Nguyên nhân:** Server chưa chạy hoặc đã dừng

**Giải pháp:**
1. Kiểm tra Command Prompt/Terminal có đang chạy server không
2. Nếu không, chạy lại: `python smart_bin_server_openai.py`
3. Đợi thông báo "Running on http://0.0.0.0:5000"

### ❌ Lỗi 8: Biểu đồ không hiển thị

**Nguyên nhân:** Chart.js không load được

**Giải pháp:**
1. Kiểm tra kết nối Internet (Chart.js load từ CDN)
2. Kiểm tra Console trong trình duyệt (F12) xem có lỗi không
3. Thử refresh trang (F5)

---

## 10. CẤU TRÚC THƯ MỤC

Sau khi chạy thành công, cấu trúc thư mục sẽ như sau:

```
trash_classication_ai/
│
├── file_code/
│   └── smart_bin_server_openai.py    # File server Flask
│
├── web/
│   ├── index.html                     # Giao diện web
│   ├── styles.css                     # File CSS
│   └── main.js                        # File JavaScript
│
├── data/                              # Tự động tạo khi chạy
│   ├── history.json                   # Lưu lịch sử phân loại
│   └── uploads/                       # Lưu ảnh đã phân loại
│       └── *.jpg                      # Các file ảnh
│
├── README.md                          # File hướng dẫn tổng quan
└── HUONG_DAN_CHAY_DU_AN.md           # File này
```

---

## 📝 GHI CHÚ QUAN TRỌNG

### ⚠️ Lưu ý bảo mật:
- **KHÔNG** commit API key lên GitHub
- **KHÔNG** chia sẻ API key với người khác
- Nếu vô tình commit key, hãy tạo key mới ngay lập tức

### 💡 Mẹo sử dụng:
- Giữ server chạy liên tục để web luôn hoạt động
- Dashboard tự động làm mới mỗi 30 giây
- Có thể click nút "🔄 Làm Mới" để cập nhật ngay lập tức

### 🔧 Tùy chỉnh:
- Thay đổi port: Sửa `port=5000` trong file server
- Thay đổi thời gian auto-refresh: Sửa `30000` (30 giây) trong `main.js`
- Thay đổi số ngày hiển thị: Sửa `range(6, -1, -1)` trong file server

---

## ✅ CHECKLIST HOÀN THÀNH

Trước khi bắt đầu, đảm bảo bạn đã:

- [ ] Cài đặt Python 3.8+
- [ ] Cài đặt các thư viện: flask, openai, pillow
- [ ] Có OpenAI API key
- [ ] Cấu hình API key trong file server
- [ ] Hiểu cách chạy server
- [ ] Biết cách truy cập web dashboard

---

## 🆘 CẦN HỖ TRỢ?

Nếu gặp vấn đề không giải quyết được:

1. Kiểm tra lại từng bước trong hướng dẫn
2. Xem phần "Xử lý lỗi thường gặp"
3. Kiểm tra Console trong trình duyệt (F12)
4. Kiểm tra log trong Command Prompt/Terminal

---

## 🎉 CHÚC MỪNG!

Nếu bạn đã chạy thành công và thấy dashboard, bạn đã hoàn thành việc setup! 

Bây giờ bạn có thể:
- Xem thống kê phân loại rác
- Theo dõi biểu đồ theo thời gian
- Xem lịch sử các lần phân loại

**Chúc bạn sử dụng thành công! 🚀**

