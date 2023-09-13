#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#define WINDOW_PIN 9
#define DHTPIN 8
#define LEDPIN 14
#define DHTTYPE 11

static char line;

DHT dht(DHTPIN, DHTTYPE);

Servo Window; // 창문을 제어할 수 있는 서보모터 객체 생성
Servo Door; // 현관문을 제어할 수 있는 서보모터 객체 생성
Servo servo_gar;

LiquidCrystal_I2C tv(0x27, 16, 2);
SoftwareSerial MP3Module(52, 53); //MP3(Rx,Tx)

int led[] = {2,3,5,7,10,11};
int tries1, tries2;
static char;

void setup() {
  Serial.begin(9600); // 시리얼 통신(컴퓨터 통신, 아두이노 통신)
  Serial2.begin(9600); //hm10

  for(int i=1; i<=11; i++){
    pinMode(led[i], OUTPUT);
  }
  
  Window.attach(WINDOW_PIN);
  Door.attach(DOOR_PIN);
  servo_gar.attach(gar_PIN);
  servo_gar.write(0); // 스마트주차장의 초기 각도는 0도

  tv.init();
  tv.backlight();

  if (!MP3Player.begin(MP3Module, false)) { // MP3 모듈을 초기화합니다. 초기화에 실패하면 오류를 발생시킵니다.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  MP3Player.volume(15);
}

void loop() {
  btncheck(); // 블루투스 연결

  float Temp = dht.readTemperature();
  float Humi = dht.readHumidity();
  
  Serial.print("Temperature : ");
  Serial.println(Temp);
  Serial.println(*);
  Serial.print("Humidity : ");
  Serial.println(Humi);
  Serial.println(*);
}

//창문 열기&&닫기
//평상시 창문을 열고 닫는 함수
void openWindow() {
  Window.write(90);
}
void closeWindow() {
  Window.write(10);
}
void openDoor() {
  Window.write(90);
}
void closeDoor() {
  Window.write(10);
}
//무드등 켜고 끄는 함수
void turnOnLights() {
  for(int i=1; i<=11; i++){
    digitalWrite(led[i], HIGH);
  }
}
void turnOffLights() {
  for(int i=1; i<=11; i++){
    digitalWrite(led[i], LOW);
  }
}
void turnOnTv(){
  tv.backlight();
}
void turnOffTv(){
  tv.noBacklight();
}
void turnOnAirConditioner(){
  digitalWrite(LEDPIN, HIGH);
  if(tries1 < 1){
    MP3Player.play(1);
    tries1++;
  }
}
void turnOffAirConditioner(){
  digitalWrite(LEDPIN, LOW)
  if (tries1 >= 1) {
      MP3Player.play(2);
  }
  tries = 0;
}
void OpenGarageDoor(){
  servo_gar.write(90);
  tv.clear();
  if (tries2 < 1) {
    MP3Player.play(3);
    lcd.print("Open"); // 모니터에 열려 있다는 것을 구현
    delay(2000);
    tries2++;
   }
}
void CloseGarageDoor(){
  servo_gar.write(0);  // 측정거리가 임계거리보다 클 시에는 서보모터 0도로 읽음
  lcd.clear();
  if (tries2 >= 1) {
     MP3Player.play(4);
     lcd.print("Close");
     delay(2000);
   }
   tries2 = 0;
}

void btncheck() {
  if (Serial.available() > 0) {
    Serial2.write(Serial.read());
    Serial2.print(line);
  }
  if (Serial2.available() > 0) {
    char s = Serial2.read(); // 시리얼 모니터로부터 들어온 데이터를 Serial1으로 출력
    if (s == 'a') {      
      turnOffLights();
    }
    if (s == 'b') {
      turnOnLights();
    }
    if (s == 'c') {
      closeWindow();
    }
    if (s == 'd') {
      openWindow();
    }
    if (S == 'e'){
      turnOffTv();
    }
    if (s == 'f'){
      turnOnTv();
    }
    if (s == 'g'){
      closeWindow();
    }
    if (s == 'h'){
      openDoor();
    }
    if (s == 'i'){
      turnOffAirConditioner();
    }
    if (s == 'j'){
      turnOnAirConditioner();
    }
    if (s == 'k'){
      CloseGarageDoor();
    }
    if (s == 'l'){
      OpenGarageDoor();
    }
  }
}
