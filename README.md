# XeBuytAnToan - Hệ Thống Đếm Người và Cảnh Báo Khói

Dự án điều khiển hệ thống an toàn trên xe bus (đếm người, phát hiện khói).

## Thông Tin Dự Án

**Board**: Arduino UNO  
**Framework**: Arduino  
**Công Cụ Build**: PlatformIO

## Cấu Trúc Thư Mục

```
XEBUYt/
├── src/
│   └── main.cpp          # Mã nguồn chính
├── include/              # Tập tin header (nếu cần)
├── lib/                  # Thư viện bên ngoài
├── platformio.ini        # Cấu hình PlatformIO
├── .gitignore           # Tập tin bỏ qua Git
└── README.md            # Tài liệu này
```

## Chân Kết Nối

### Output Pins
- **Pin 8**: LATCH_PIN (Chân ST_CP của 74HC595)
- **Pin 7**: CLOCK_PIN (Chân SH_CP của 74HC595)
- **Pin 6**: DATA_PIN (Chân DS của 74HC595)
- **Pin 4**: BUZZER_PIN (Còi báo động 5V)
- **Pin 5**: FAN_PIN (Quạt 5V)
- **Pin 9**: MOTION_PIN (Cảm biến chuyển động)
- **Pin 10**: KEY_PIN (Chìa khóa)

### Input Pins
- **Pin 2**: SENSOR_ENTER (Cảm biến tiệm cận đếm người lên)
- **Pin 3**: SENSOR_EXIT (Cảm biến tiệm cận đếm người xuống)
- **Pin A0**: SMOKE_SENSOR_PIN (Cảm biến khói MP-2)

## Cấu Hình Upload

Chỉnh sửa file `platformio.ini` để đặt cổng COM phù hợp:
```ini
upload_port = COM3
```

## Lệnh Build và Upload

Nếu sử dụng VS Code với PlatformIO extension:
- **Build**: Ctrl+Alt+B
- **Upload**: Ctrl+Alt+U
- **Monitor Serial**: Ctrl+Alt+S

Hoặc sử dụng dòng lệnh:
```bash
pio run -t upload
```

## Thay Đổi So Với Arduino IDE

- File `.ino` được chuyển thành `src/main.cpp`
- Thêm khai báo hàm ở đầu file (function declarations)
- Cấu hình board trong `platformio.ini` thay vì Arduino IDE GUI
- Có khả năng quản lý thư viện tốt hơn thông qua `platformio.ini`

## Ghi Chú

Nếu sử dụng board khác (Arduino Mega, ESP8266, v.v.), hãy cập nhật `platformio.ini`:
```ini
[env:mega2560]
platform = atmelavr
board = megaatmega2560
framework = arduino
```
