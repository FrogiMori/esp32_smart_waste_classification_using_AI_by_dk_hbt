#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// ===== WIFI =====
const char* WIFI_SSID = "tên wifi";      // <-- SỬA
const char* WIFI_PASS = "pass wifi";  // <-- SỬA

// ===== SERVER (IP laptop chạy Python) =====
// Ví dụ: "http://192.168.1.11:5000/classify"
String SERVER_URL = "http://<dịa chỉ ip>:5000/classify";  // <-- SỬA IP

// ===== CHÂN CAMERA AI THINKER =====
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===== HC-SR04 =====
const int TRIG_PIN = 12;
const int ECHO_PIN = 13;

// ===== SERVO TẤM CHẮN =====
const int SERVO_PIN = 14;
Servo flapServo;

// Góc theo tính toán của bạn:
// 90°: giữa, 35°: mở NGĂN TRÁI (RECYCLE), 145°: mở NGĂN PHẢI
int ANGLE_CENTER     = 90;
int ANGLE_LEFT_OPEN  = 35;   // RECYCLE -> ngăn TRÁI
int ANGLE_RIGHT_OPEN = 145;  // ORGANIC/OTHER -> ngăn PHẢI

const unsigned long FLAP_OPEN_TIME = 2500; // ms giữ tấm chắn mở

// ===== THỜI GIAN & NGƯỠNG =====
const unsigned long CHECK_INTERVAL  = 200;   // đọc sensor mỗi 200 ms
const unsigned long MIN_TRIGGER_GAP = 5000;  // tối thiểu 5 s giữa 2 lần gọi AI
unsigned long lastCheckMillis       = 0;
unsigned long lastTriggerMillis     = 0;

const float TRIGGER_DISTANCE_CM = 18.0;      // < 18 cm thì kích hoạt

// ---------------- WIFI ----------------
void connectWiFi() {
  Serial.print("Dang ket noi WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi OK, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi FAILED – van co the chay che khong co AI");
  }
}

// ---------------- CAMERA ----------------
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size   = FRAMESIZE_QVGA;  // 320x240
  config.jpeg_quality = 12;
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Loi init camera 0x%x\n", err);
    return false;
  }
  Serial.println("Camera OK");
  return true;
}

// ---------------- ĐỌC KHOẢNG CÁCH ----------------
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30 ms ~ 5 m
  if (duration == 0) return -1;
  float distance = duration * 0.0343 / 2.0;
  return distance;
}

// ---------------- GỌI SERVER AI ----------------
String captureAndClassify() {
  Serial.println("Dang chup anh...");
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Khong lay duoc frame");
    return "ERROR_NO_FRAME";
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Mat WiFi, khong gui duoc");
    esp_camera_fb_return(fb);
    return "NO_WIFI";
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "image/jpeg");

  int code = http.POST(fb->buf, fb->len);
  String label = "ERROR_HTTP";

  if (code > 0) {
    Serial.printf("HTTP code: %d\n", code);
    if (code == HTTP_CODE_OK) {
      label = http.getString();
      label.trim();
      label.toUpperCase();
      Serial.print("AI tra ve: ");
      Serial.println(label);
    } else {
      Serial.println("Server khong tra 200.");
    }
  } else {
    Serial.printf("Loi POST: %s\n", http.errorToString(code).c_str());
  }

  http.end();
  esp_camera_fb_return(fb);
  return label;
}

// ---------------- SERVO THEO NHÃN ----------------
void moveFlapForLabel(const String &label) {
  // RECYCLE  -> NGĂN TRÁI (tái chế)
  // ORGANIC / OTHER -> NGĂN PHẢI
  if (label == "RECYCLE") {
    Serial.println("=> Mo NGAN TAI CHE (TRAI)");
    flapServo.write(ANGLE_LEFT_OPEN);   // 35°
  } else {
    Serial.println("=> Mo NGAN HUU CO/OTHER (PHAI)");
    flapServo.write(ANGLE_RIGHT_OPEN);  // 145°
  }

  delay(FLAP_OPEN_TIME);

  Serial.println("=> Dua TAM CHAN ve GIUA");
  flapServo.write(ANGLE_CENTER);        // 90°
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n===== SMART BIN (AI + HC-SR04 + SERVO) =====");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  flapServo.attach(SERVO_PIN);
  flapServo.write(ANGLE_CENTER);

  connectWiFi();
  initCamera();
}

// ---------------- LOOP ----------------
void loop() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (now - lastCheckMillis >= CHECK_INTERVAL) {
    lastCheckMillis = now;

    float d = readDistanceCm();
    if (d > 0) {
      Serial.print("Khoang cach: ");
      Serial.print(d);
      Serial.println(" cm");
    }

    if (d > 0 && d < TRIGGER_DISTANCE_CM &&
        (now - lastTriggerMillis > MIN_TRIGGER_GAP)) {

      lastTriggerMillis = now;
      Serial.println("==> Phat hien RAC, goi AI...");

      String label = captureAndClassify();
      Serial.print("Ket qua AI: ");
      Serial.println(label);

      moveFlapForLabel(label);
      Serial.println("----------------------------");
    }
  }

  delay(20);
}
