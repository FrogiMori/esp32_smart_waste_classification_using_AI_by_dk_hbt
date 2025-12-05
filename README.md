# 🗑️ Smart Trash Classification AI - Dashboard Web

Hệ thống phân loại rác thông minh sử dụng AI với giao diện web dashboard đầy đủ tính năng.

## ✨ Tính Năng

### 📤 Upload & Phân Loại
- Upload ảnh rác bằng cách kéo thả hoặc chọn file
- Phân loại tự động bằng OpenAI Vision API
- Hiển thị kết quả với màu sắc phân biệt

### 📊 Dashboard Thống Kê
- **Thẻ thống kê**: Hiển thị số lượng và tỷ lệ % từng loại rác
- **Biểu đồ đường**: Phân loại theo thời gian (7 ngày gần nhất)
- **Biểu đồ tròn**: Tỷ lệ phân loại trực quan
- **Lịch sử**: Danh sách các lần phân loại gần đây với ảnh

### 🎨 Giao Diện
- Thiết kế hiện đại, responsive
- Màu sắc phân biệt rõ ràng cho từng loại rác
- Animation mượt mà
- Tự động làm mới dữ liệu mỗi 30 giây

## 🚀 Cài Đặt

### 1. Cài đặt thư viện

```bash
pip install flask openai pillow
```

### 2. Cấu hình API Key

Mở file `file_code/smart_bin_server_openai.py` và thay thế:

```python
client = OpenAI(api_key="YOUR_OPENAI_API_KEY_HERE")
```

Bằng API key của bạn:

```python
client = OpenAI(api_key="sk-your-actual-api-key-here")
```

### 3. Chạy Server

```bash
cd file_code
python smart_bin_server_openai.py
```

Server sẽ chạy tại: `http://localhost:5000/`

## 📁 Cấu Trúc Thư Mục

```
trash_classication_ai/
├── file_code/
│   └── smart_bin_server_openai.py  # Flask server
├── web/
│   ├── index.html                  # Giao diện web
│   ├── styles.css                  # Styling
│   └── main.js                     # JavaScript logic
├── data/
│   ├── history.json                # Lịch sử phân loại (tự động tạo)
│   └── uploads/                    # Thư mục lưu ảnh (tự động tạo)
└── README.md
```

## 🔌 API Endpoints

### 1. Dashboard Web
- **GET** `/` - Trang chủ dashboard

### 2. Phân Loại
- **POST** `/classify` - API cho ESP32 (raw image bytes)
- **POST** `/classify-web` - API cho web upload (multipart/form-data)

### 3. Thống Kê & Lịch Sử
- **GET** `/api/statistics` - Lấy thống kê và biểu đồ
- **GET** `/api/history?limit=20` - Lấy lịch sử phân loại
- **GET** `/api/image/<filename>` - Lấy ảnh đã lưu

## 📊 Các Loại Rác

- **RECYCLE** ♻️: Chai nhựa, lon, giấy, bìa carton, bao bì nhựa sạch...
- **ORGANIC** 🍃: Thức ăn thừa, vỏ trái cây, lá cây, rác hữu cơ...
- **OTHER** 🗑️: Rác thải hỗn hợp, không thể tái chế, bao bì bẩn...

## 🎯 Sử Dụng

1. Mở trình duyệt và truy cập: `http://localhost:5000/`
2. Kéo thả ảnh rác vào vùng upload hoặc click để chọn file
3. Click nút "Phân Loại" để bắt đầu
4. Xem kết quả và thống kê trên dashboard

## 🔧 Tùy Chỉnh

### Thay đổi số ngày hiển thị biểu đồ
Sửa trong `file_code/smart_bin_server_openai.py`:

```python
for i in range(6, -1, -1):  # Thay 6 thành số ngày bạn muốn
```

### Thay đổi số lượng lịch sử lưu trữ
Sửa trong `file_code/smart_bin_server_openai.py`:

```python
history = history[:100]  # Thay 100 thành số lượng bạn muốn
```

### Thay đổi thời gian tự động làm mới
Sửa trong `web/main.js`:

```javascript
setInterval(() => {
    loadStatistics();
    loadHistory();
}, 30000);  // Thay 30000 (30 giây) thành thời gian bạn muốn (ms)
```

## 📝 Lưu Ý

- Đảm bảo có kết nối internet để sử dụng OpenAI API
- Ảnh được lưu trong thư mục `data/uploads/` để xem lại
- Lịch sử được lưu trong file JSON `data/history.json`
- Server chạy ở chế độ debug để dễ phát triển

## 🐛 Xử Lý Lỗi

- Nếu không kết nối được server: Kiểm tra xem Flask server đã chạy chưa
- Nếu lỗi API: Kiểm tra API key OpenAI có đúng không
- Nếu không hiển thị ảnh: Kiểm tra quyền ghi file trong thư mục `data/`

## 📄 License

Dự án này được phát triển cho mục đích giáo dục.

