// Định nghĩa các chân kết nối
// Mạch led 7 đoạn 74HC595
#define LATCH_PIN 8  // Chân ST_CP của 74HC595
#define CLOCK_PIN 7  // Chân SH_CP của 74HC595
#define DATA_PIN 6   // Chân DS của 74HC595

// Cảm biến
#define SENSOR_ENTER 2       // Cảm biến tiệm cận đếm người lên
#define SENSOR_EXIT 3        // Cảm biến tiệm cận đếm người xuống
#define SMOKE_SENSOR_PIN A0  // Cảm biến khói MP-2

// Thiết bị đầu ra
#define BUZZER_PIN 4  // Còi báo động 5V
#define FAN_PIN 5     // Quạt 5V
#define MOTION_PIN 9  //cam bien chuyen dong
#define KEY_PIN 10    //chia khoa
// #define RELAY_PIN 6     // Module relay điều khiển đèn

// Ngưỡng cảm biến khói
#define SMOKE_THRESHOLD 950

// Biến toàn cục
int passengerCount = 0;      // Số lượng người trên xe
bool smokeDetected = false;  // Trạng thái phát hiện khói
bool lastEnterState = HIGH;  // Trạng thái trước đó của cảm biến vào
bool lastExitState = HIGH;   // Trạng thái trước đó của cảm biến ra
bool enterTriggered = false;
bool exitTriggered = false;
bool motionTriggered = false;
bool keyTriggered = false;
bool motionTemp = false;
bool smokeTemp = false;
bool keyTemp = false;
bool activeMotion = false;
unsigned long smokeStartTime = 0;
unsigned long smokeEndTime = 0;
unsigned long keyTime = 0;
const unsigned long SMOKE_DELAY = 2000;      // 5 giây
const unsigned long SMOKE_DELAY_OFF = 5000;  // 5 giây
unsigned long motionStartTime = 0;
const unsigned long MOTION_DELAY = 5000;  // 5 giây
const unsigned long KEY_DELAY = 2000;     // 5 giây
// Mảng mã LED 7 đoạn (0-9) - Common Cathode
const byte digitPattern[10] = {
  B11000000,  //0
  B11111001,  //1
  B10100100,  //2
  B10110000,  //3
  B10011001,  //4
  B10010010,  //5
  B10000010,  //6
  B11111000,  //7
  B10000000,  //8
  B10010000   //9
};

void setup() {
  // Khởi tạo Serial để debug
  Serial.begin(9600);

  // Cấu hình các chân output
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  // pinMode(RELAY_PIN, OUTPUT);

  // Cấu hình các chân input
  pinMode(SENSOR_ENTER, INPUT);
  pinMode(SENSOR_EXIT, INPUT);
  pinMode(SMOKE_SENSOR_PIN, INPUT);
  pinMode(MOTION_PIN, INPUT);
  pinMode(KEY_PIN, INPUT);
  // Khởi tạo trạng thái ban đầu
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  // digitalWrite(RELAY_PIN, LOW);

  // Hiển thị số người ban đầu (0)
  updateDisplay(passengerCount);

  Serial.println("Hệ thống đếm người và cảnh báo khói đã khởi động!");
}

void loop() {
  // Đọc tín hiệu từ các cảm biến
  bool currentEnterState = !digitalRead(SENSOR_ENTER);
  bool currentExitState = !digitalRead(SENSOR_EXIT);
  bool motionState = digitalRead(MOTION_PIN);
  bool keyState = digitalRead(KEY_PIN);
  int smokeValue = analogRead(SMOKE_SENSOR_PIN);
  Serial.println("-----");
  Serial.println(smokeValue);
  Serial.print(currentEnterState);
  Serial.print(currentExitState);
  Serial.println("-----");
  Serial.print(enterTriggered);
  Serial.print(exitTriggered);
  Serial.print(passengerCount);
  Serial.println("MOTION");
  Serial.print(motionState);
  Serial.print(motionTriggered);
  Serial.println("-----");
  // Kiểm tra người lên xe (phát hiện cạnh xuống - người đi qua cảm biến)

  //////////////////////////////////////////////////////////////////////
  // Xử lý khi có người lên xe

  if (enterTriggered && currentExitState && !exitTriggered) {
    exitTriggered = true;  // Xác nhận cảm biến ra đã kích hoạt sau cảm biến vào
    passengerCount++;      // Tăng số lượng người
    Serial.print("Người lên xe. Tổng số người: ");
    Serial.println(passengerCount);
    updateDisplay(passengerCount);
  }

  // Xử lý khi có người xuống xe

  if (exitTriggered && currentEnterState && !enterTriggered) {
    enterTriggered = true;                     // Xác nhận cảm biến vào đã kích hoạt sau cảm biến ra
    if (passengerCount > 0) passengerCount--;  // Giảm số lượng người nếu > 0
    Serial.print("Người xuống xe. Tổng số người: ");
    Serial.println(passengerCount);
    updateDisplay(passengerCount);
  }
  if (currentEnterState && !enterTriggered) {
    enterTriggered = true;  // Đánh dấu cảm biến vào đã kích hoạt
    exitTriggered = false;
  } else if (currentExitState && !exitTriggered) {
    exitTriggered = true;  // Đánh dấu cảm biến ra đã kích hoạt
    enterTriggered = false;
    Serial.print("exitTriggered");
  }
  // Reset trạng thái khi không còn ai đi qua
  if (!currentEnterState && !currentExitState) {
    enterTriggered = false;
    exitTriggered = false;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Kiểm tra cảm biến khói

  // Xử lý cảm biến khói với độ trễ 5 giây
  if (smokeValue > SMOKE_THRESHOLD) {
    if (smokeStartTime == 0) {
      smokeStartTime = millis();  // Bắt đầu đếm thời gian
    }
    if (millis() - smokeStartTime >= SMOKE_DELAY) {
      smokeDetected = true;  // Chuyển trạng thái sau 5 giây
      Serial.print(smokeValue);
      Serial.println("🔥 Cảnh báo: Phát hiện khói! 🔥");
    }
  } else {
    if (smokeTemp == false && smokeDetected == true) {
      smokeEndTime = millis();  // Bắt đầu đếm thời gian
      smokeTemp = true;
    }
    if (millis() - smokeEndTime >= SMOKE_DELAY_OFF) {
      smokeStartTime = 0;  // Reset bộ đếm thời gian nếu không còn khói
      smokeEndTime = 0;
      smokeDetected = false;
      smokeTemp = false;
    }
  }
  // Xử lý cảnh báo khi có khói và có người trên xe
  if (smokeDetected && passengerCount > 0 || activeMotion == true) {
    activateAlarm(true);
  } else {
    activateAlarm(false);
  }
  //// XU LY CAM BIEN CHUYEN DONG
  if (keyState == true && keyTemp == false) {
    keyTime = millis();
    keyTemp = true;
  }
  if (keyTemp == true && ((millis() - keyTime) > KEY_DELAY)){
      keyTriggered = true;
    }
  if (keyState == false) {
    keyTriggered = false;
    keyTemp = false;
    keyTime = 0;
  }

  if (motionState == HIGH && motionTriggered == false && keyTriggered == true) {
    motionTriggered = true;
  }
  if (motionTriggered == true && motionTemp == false) {
    activeMotion = true;
    motionStartTime = millis();
    motionTemp = true;
  }
  if (motionTemp == true && ((millis() - motionStartTime) > MOTION_DELAY)) {
    activeMotion = false;
    motionTemp = false;
    motionTriggered = false;
  }
  //////////////////////////////////
  // Thời gian làm mới vòng lặp
  delay(100);
}

// Hàm kích hoạt cảnh báo
void activateAlarm(bool activate) {
  if (activate) {
    digitalWrite(BUZZER_PIN, HIGH);  // Bật còi
    digitalWrite(FAN_PIN, HIGH);     // Bật quạt để thông gió
    // digitalWrite(RELAY_PIN, HIGH);  // Đóng relay bật đèn
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Tắt còi
    digitalWrite(FAN_PIN, LOW);     // Tắt quạt
    // digitalWrite(RELAY_PIN, LOW);   // Ngắt relay tắt đèn
  }
}

// Hàm hiển thị số lượng người trên LED 7 đoạn
void updateDisplay(int count) {
  // Giới hạn số người hiển thị tối đa là 99
  if (count > 99) count = 99;
  if (count < 0) count = 0;

  // Tách các chữ số
  int tens = count / 10;
  int ones = count % 10;

  // Hiển thị lên LED 7 đoạn
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitPattern[ones]);  // Gửi chữ số hàng đơn vị
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitPattern[tens]);  // Gửi chữ số hàng chục
  digitalWrite(LATCH_PIN, HIGH);
}
