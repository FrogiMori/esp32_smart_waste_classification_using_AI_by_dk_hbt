from flask import Flask, request, send_from_directory, jsonify
from openai import OpenAI
import base64
import io
import os
import json
from datetime import datetime, timedelta
from PIL import Image
import traceback
from collections import defaultdict

app = Flask(__name__, static_folder='../web', static_url_path='')

# ==== ĐIỀN API KEY CỦA BẠN VÀO ĐÂY ====
client = OpenAI(api_key="YOUR_OPENAI_API_KEY_HERE")

# File lưu lịch sử
HISTORY_FILE = "data/history.json"
STATS_FILE = "data/statistics.json"

# Đảm bảo thư mục data tồn tại
os.makedirs("data", exist_ok=True)
os.makedirs("data/uploads", exist_ok=True)

def load_history():
    """Load lịch sử từ file JSON"""
    if os.path.exists(HISTORY_FILE):
        try:
            with open(HISTORY_FILE, 'r', encoding='utf-8') as f:
                return json.load(f)
        except:
            return []
    return []

def save_history(history):
    """Lưu lịch sử vào file JSON"""
    try:
        with open(HISTORY_FILE, 'w', encoding='utf-8') as f:
            json.dump(history, f, ensure_ascii=False, indent=2)
    except Exception as e:
        print(f"Lỗi lưu lịch sử: {e}")

def add_to_history(label, image_bytes=None):
    """Thêm một bản ghi vào lịch sử"""
    history = load_history()
    
    # Lưu ảnh nếu có
    image_url = None
    if image_bytes:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
        image_filename = f"{timestamp}.jpg"
        image_path = os.path.join("data/uploads", image_filename)
        try:
            with open(image_path, 'wb') as f:
                f.write(image_bytes)
            image_url = f"/api/image/{image_filename}"
        except Exception as e:
            print(f"Lỗi lưu ảnh: {e}")
    
    entry = {
        "label": label,
        "timestamp": datetime.now().isoformat(),
        "image_url": image_url
    }
    
    history.insert(0, entry)  # Thêm vào đầu
    # Giữ tối đa 100 bản ghi gần nhất
    history = history[:100]
    save_history(history)
    
    return entry

def classify_with_openai(image_bytes: bytes) -> str:
    """
    Nhận bytes ảnh JPEG, gửi lên OpenAI bằng chat.completions,
    trả về RECYCLE / ORGANIC / OTHER.
    Nếu lỗi thì in ra console và trả về OTHER (tránh HTTP 500).
    """
    # kiểm tra ảnh (không bắt buộc)
    try:
        img = Image.open(io.BytesIO(image_bytes))
        img.verify()
    except Exception as e:
        print("Ảnh không hợp lệ (vẫn thử gửi OpenAI):", e)

    # mã hoá base64 thành data URL
    b64 = base64.b64encode(image_bytes).decode("utf-8")
    data_url = f"data:image/jpeg;base64,{b64}"

    system_prompt = (
        "You are an AI helping a smart trash bin in a high school.\n"
        "Look at the photo and classify the MAIN waste item into exactly ONE of these 3 categories:\n"
        "- RECYCLE: plastic bottles, cans, paper, cardboard, clean plastic packaging, etc.\n"
        "- ORGANIC: food scraps, fruit/vegetable peels, leaves, leftover food, etc.\n"
        "- OTHER: mixed or dirty non-recyclable waste such as snack bags, dirty styrofoam boxes, etc.\n"
        "Answer with ONLY one word in ENGLISH and UPPERCASE:\n"
        "RECYCLE or ORGANIC or OTHER."
    )

    try:
        # GỌI CHAT.COMPLETIONS (vision) – KHÔNG DÙNG responses.create nữa
        response = client.chat.completions.create(
            model="gpt-4.1-mini",
            messages=[
                {"role": "system", "content": system_prompt},
                {
                    "role": "user",
                    "content": [
                        {
                            "type": "text",
                            "text": "Here is a photo of the waste item. Classify it."
                        },
                        {
                            "type": "image_url",
                            "image_url": {"url": data_url}
                        },
                    ],
                },
            ],
        )

        raw = response.choices[0].message.content.strip().upper()
        print("[OpenAI raw]:", raw)

        if "RECYCLE" in raw:
            return "RECYCLE"
        if "ORGANIC" in raw:
            return "ORGANIC"
        if "OTHER" in raw:
            return "OTHER"

        # nếu text lạ, fallback OTHER
        return "OTHER"

    except Exception as e:
        print("LỖI khi gọi OpenAI (chat.completions):")
        print(e)
        traceback.print_exc()
        # Fallback: luôn trả OTHER để ESP32 không bị HTTP 500
        return "OTHER"


@app.route("/")
def index():
    """Serve trang web chính"""
    return send_from_directory('../web', 'index.html')


@app.route("/classify", methods=["POST"])
def classify():
    """API cho ESP32 - nhận raw image bytes"""
    img_bytes = request.get_data()
    if not img_bytes:
        print("Không nhận được ảnh từ ESP32")
        return "NO_IMAGE", 400

    label = classify_with_openai(img_bytes)
    print("[SERVER] Trả về nhãn:", label)
    
    # Lưu vào lịch sử
    add_to_history(label, img_bytes)
    
    # Luôn trả HTTP 200
    return label, 200


@app.route("/classify-web", methods=["POST"])
def classify_web():
    """API cho web - nhận file upload"""
    if 'image' not in request.files:
        return jsonify({"error": "No image file"}), 400
    
    file = request.files['image']
    if file.filename == '':
        return jsonify({"error": "No file selected"}), 400
    
    try:
        img_bytes = file.read()
        label = classify_with_openai(img_bytes)
        print("[SERVER] Web upload - Trả về nhãn:", label)
        
        # Lưu vào lịch sử
        add_to_history(label, img_bytes)
        
        return jsonify({"label": label}), 200
    except Exception as e:
        print(f"Lỗi xử lý ảnh: {e}")
        traceback.print_exc()
        return jsonify({"error": str(e)}), 500


@app.route("/api/statistics", methods=["GET"])
def get_statistics():
    """API trả về thống kê"""
    history = load_history()
    
    # Đếm theo loại
    counts = defaultdict(int)
    for entry in history:
        label = entry.get("label", "OTHER")
        counts[label] += 1
    
    recycle = counts.get("RECYCLE", 0)
    organic = counts.get("ORGANIC", 0)
    other = counts.get("OTHER", 0)
    total = recycle + organic + other
    
    # Tính toán theo thời gian (7 ngày gần nhất)
    time_series = []
    today = datetime.now().date()
    
    for i in range(6, -1, -1):
        date = today - timedelta(days=i)
        date_str = date.isoformat()
        
        day_counts = defaultdict(int)
        for entry in history:
            entry_date = datetime.fromisoformat(entry["timestamp"]).date()
            if entry_date == date:
                label = entry.get("label", "OTHER")
                day_counts[label] += 1
        
        time_series.append({
            "date": date_str,
            "recycle": day_counts.get("RECYCLE", 0),
            "organic": day_counts.get("ORGANIC", 0),
            "other": day_counts.get("OTHER", 0)
        })
    
    return jsonify({
        "recycle": recycle,
        "organic": organic,
        "other": other,
        "total": total,
        "timeSeries": time_series
    })


@app.route("/api/history", methods=["GET"])
def get_history():
    """API trả về lịch sử phân loại"""
    limit = request.args.get('limit', 20, type=int)
    history = load_history()
    
    # Trả về số lượng giới hạn
    return jsonify(history[:limit])


@app.route("/api/image/<filename>", methods=["GET"])
def get_image(filename):
    """API trả về ảnh đã lưu"""
    try:
        return send_from_directory("data/uploads", filename)
    except:
        return "Image not found", 404


if __name__ == "__main__":
    print("=" * 50)
    print("SMART BIN AI SERVER (OpenAI)")
    print("=" * 50)
    print("Server chạy tại: http://localhost:5000/")
    print("API endpoints:")
    print("  - GET  /              -> Dashboard web")
    print("  - POST /classify      -> API cho ESP32")
    print("  - POST /classify-web  -> API cho web upload")
    print("  - GET  /api/statistics -> Thống kê")
    print("  - GET  /api/history   -> Lịch sử")
    print("=" * 50)
    app.run(host="0.0.0.0", port=5000, debug=True)
