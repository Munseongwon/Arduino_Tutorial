#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

DHT_Unified dht(8, DHT11); // 에어컨을 켜기 위한 온습도계에 사용할 온습도핀, 식물 성장 감지시스템에서 역시 사용

LiquidCrystal_I2C tv(0x27,16,2);
LiquidCrystal_I2C Monitor(0x27, 16,2);

#define BT_RXD 5    // 아두이노의 5번 핀을 RX핀 지정! 블루투스와 TX핀 연결(블루투스 보내는 핀)
#define BT_TXD 4    // 아두이노의 4번 핀을 TX핀 지정! 블루투스의 RX핀 연결(블루투스 받는 핀)
SoftwareSerial bluebooth(BT_RXD,BT_TXD);

SoftwareSerial MP3Module(7,8);
DFRobotDFPlayerMini MP3Player;

//스마트 에어컨 전역 변수
//const int pinAirConditioner = ?;
//const int pinPiezo = ?;
#define LEDPIN = 3
#define Temp_C 26.0

int tries; // 에어컨이 동작한 횟수

float humidity, temperature;
//float discomfortIndex = 0.; //불쾌지수의 기본값

//boolean isAirConditionerTurnOn = false; // 에어컨이 켜져 있는지 확인

//스마트 주차장 전역 변수
const int TRIG_PIN  = 6;  // Arduino pin connected to Ultrasonic Sensor's TRIG pin(초음파 센서 트리거 핀 연결)
const int ECHO_PIN  = 7;  // Arduino pin connected to Ultrasonic Sensor's ECHO pin(초음파 센서 에코 핀 연결)
const int SERVO_PIN = 9; // Arduino pin connected to Servo Motor's pin(주차장 서보모터 센서 핀 연결)
const int DISTANCE_THRESHOLD = 30; // centimeters

float duration_us, distance_cm;

//스마트 도어 전역 변수
const int servo_pin = 13;                     // 서보의 제어핀 13번 핀으로 선언
int init_angle = 0;                          // 서보의 각도 초기값
//int angle = init_angle;                       // 서보의 각도를 저장하기 위한 변수
//int control_num = 0;                          // 도어락의 상태를 저장하기 위한 변수

//식물 성장 감지 시스템 전역 변수
int led[] = {10, 11, 12};

//스마트 무드등 전역 변수
//boolean isLightTurnOffByUser = false; // 전등이 꺼져 있는 지 확인

//화재경보시스템 전역 변수
const int fire = 1; // 불꽃 감지 센서 연결 핀

//지진경보시스템 전역 변수
const int vibr = 2;

//경보음
int notes[] = {262, 294, 330, 349, 392, 440, 494, 523};

//피에조 스피커
int speaker = 5;

//각 서보모터 각도 초기값 설정
int pos1 = 0;
int pos2 = 0;

//각 서보모터에 대한 객체 생성
Servo Garservo; // 스마트 주차장을 제어할 수 있는 서보모터 객체 생성
Servo Doorservo; // 스마트 문을 제어할 수 있는 서보모터 객체 생성
Servo window; // 창문을 제어할 수 있는 서보모터 객체 생성

void setup() {
  // put your setup code here, to run once:
  
  //식물 성장 감시 시스템, 비상 상황 시 켜지는 불
  for(int i=1; i<=3; i++){
    pinMode(led[i], OUTPUT);  
  }
  
  //스마트 주차장
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
  servo.write(0);

  //스마트 도어
  Doorservo.attach(servo_pin);

  //무드등
  for(int i= 2; i<=7; i++){
    pinMode(i, OUTPUT);
  }

  //스마트에어컨
  pinMode(LEDPIN, OUTPUT);
  dht.begin();
  MP3Module.begin(9600);
   if (!MP3Player.begin(MP3Module,false)) { // MP3 모듈을 초기화합니다. 초기화에 실패하면 오류를 발생시킵니다.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  MP3Player.volume(15); // 음성 볼륨의 크기는 15로 설정

  //화재 발생 시 혹은 지진 발생 시 lcd 초기화
  pinMode(fire, INPUT); // 불꽃감지 센서를 입력모듈로 설정
  pinMode(vibr, INPUT);
  
  tv.init();
  tv.backlight();
  
  //블루투스 통신을 위한 시리얼
  bluetooth.begin(9600);
}

//응급 상황 발생 시 led 깜빡거림
void blink_LED(){
  digitalWrite(led[1], HIGH);
  delay(5000);
  digitalWrite(led[1], LOW);
  delay(5000);
}

//식물의 토양 수분 상태에 대한 함수 -> 충분하면 초록빛 led를 부족하면 빨간색 led를 많으면 파란색 led
void Many(){
  digitalWrite(led[3], HIGH);
  delay(3000);
  digitalWrite(led[3], LOW);
  delay(3000);
}
void Enough(){
  digitalWrite(led[2], HIGH);
  delay(3000);
  digitalWrite(led[2], LOW);
  delay(3000);
}
void lack(){
  digitalWrite(led[1], HIGH);
  delay(3000);
  digitalWrite(led[1], LOW);
  delay(3000);
}

void PlantGrawingSystemOn(int WaterOfSoil, int HumidOfSoil){
   if((waterOfSoil>=200 && waterOfSoil<499) || (humidityOfIndoor>=50 && humidityOfIndoor<=80) || (tempOfIndoor>=18 && tempOfIndoor<=26){
      Enough();
   }
   else if((waterOfSoil<200) || (humidityOfIndoor<50) || (tempOfIndoor<18)){
      Low();
   }
   else if((waterOfSoil>=499) || (humidityOfIndoor>80) || (tempOfIndoor>26)){
      High();
   }
}

void PlantGrawingSystemOff(){
  for(int i=1; i<=3; i++){
    digitalWrite(led[i],LOW); 
  }
}

//TV 켜는 함수
void turnOnTV(){
  TV.setCursor(0,0);
  TV.backlight();
  TV.print("Hello!");
  isTVTurnOn = true; 
}

//TV 끄는 함수
void turnOffTV(){
  TV.clear();
  TV.noBacklight();
  isTVTurnOn = false;
}

//무드등 켜고 끄는 함수
void turnOnLights(){
  for(int i = 2; i<=7; i++){
    digitalWrite(i, HIGH);
    delay(1000);
  }
}
void turnOffLights(){
  for(int i=2; i<=7; i++){
    digitalWrite(i, LOW);
    delay(1000);
  }
}

//스마트 도어 함수
void OpenSmartDoor(){
  
}
void CloseSmartDoor(){
  
}

//화재가 발생했을 시 실행하는 함수 --> 화재 발생 여부에 따라 호출할 것도 다름
void OnFire(){
  tv.clear();
  tv.setCursor(1,0);
  tv.print("Warning!");
  tv.setCursor(1,1);
  tv.print("Fire!");

  for(pos1 = 0; pos1 <= 90; pos1+=30){
    window.write(pos1);
  }

  for(int i=262; i<=523; i++){
    tone(speaker, i);
    delay(10);
  }

  for(int i=523; i>=262; i--){
    tone(speaker, i);
    delay(10);
  }

  noTone(speaker);
}
void OffFire(){
  tv.clear();
  tv.setCuror(1,0);
  tv.print("No Fire!!");
}

//평상시 창문을 열고 닫는 함수
void openWindow(){
  window.attach(6);
  window.write(180);  
}
void closeWindow(){
  window.attach(6);
  window.write(0); 
}

//지진이 발생하였을 경우, 지진이 발생하지 않을 경우
void EarthquakeOccur(){
  tv.clear();
  tv.setCursor(1,0);
  tv.print("Warning!");
  tv.setCursor(1,1);
  tv.print("Earthquake!");
}
void NoEarthquakeOccur(){
  tv.clear();
  tv.setCursor(1,0);
  tv.print("No Occur");

  for(pos2 = 0; pos2 <=90; pos += 30){
    Doorservo.write(pos2);
  }

  for(int i=262; i<=523; i++){
    tone(speaker, i);
    delay(10);
  }

  for(int i=523; i>=262; i--){
    tone(speaker, i);
    delay(10);
  }
  noTone(speaker);
}

//스마트 에어컨 켜고 끄는 함수
void turnOnAirConditioner(){
  digitalWrite(LEDPIN, HIGH);
  //에어컨이 꺼져있는 상태였을 시 음악 재생
  if(tries < 1){
    MP3Player.play(1);
    tries++;
  }
}
void turnOffAirConditioner(){
  digitalWrite(LEDPIN, LOW); // 에어컨이 꺼질 시 led 꺼짐
  if(tries >=1){ // 에어컨이 켜진 상태(즉 1번 이상이라도 켜져 있다면)
     MP3Player.play(2); // 2번 음악 재생(동작 중지 음악)
  }
  tries = 0; // 다시 원래대로 꺼져 있는 상태로 되돌아옴
}

//차량 들어옴: 초음파 센서 주차할 차량 감지(초음파로 거리 측정) --> 차량이 차고 가까이 접근(특정 거리 측정) --> 서보 모터로 차고 문 열림
//차량 나감: 초음파 센서 나가는 차량 감지(장애물 감지 센서 필요) --> 차량이 차고 가까이 접근 --> 서보 모터로 차고 문 열림
void openBarricade(){
    garservo.write(90); // 측정거리가 임계거리보다 작을 때 서보모터 90도로 읽음
    Monitor.clear(); // 모니터에 남아있는 글자가 있으면 모두 삭제
    Monitor.print("Open"); // 모니터에 열려 있다는 것을 구현
    delay(1000); // 1초간의 딜레이를 줌 
}
void closeBarricade(){
    garservo.write(0);  // 측정거리가 임계거리보다 클 시에는 서보모터 0도로 읽음
    Monitor.clear(); 
    Monitor.print("Close");
    delay(2500); //2.5초간의 딜레이를 줌
}

void loop() {
  // put your main code here, to run repeatedly:
  
      //재난 상황 발생 시
      fire_state = digitalWrite(flame); // 불꽃 감지 즉 화재 발생
      vib_state = digitalWrite(vibr); // 진동 발생
      
      // 수동 식물 성장 감지 시스템 작동
      int WaterOfSoil = analogRead(A1);
      int HumidOfSoil = dht1.readHumidity();

      //스마트 주차장
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, HIGH);

      // measure duration of pulse from ECHO pin
      duration_us = pulseIn(ECHO_PIN, HIGH);
      // calculate the distance
      distance_cm = 0.02 * duration_us;

      //스마트 에어컨
      float Temp = dht.readTemperature(temperature); // dht11센서를 이용하여 온도 측정
      float Humi = dht.readHumidity(humidity); // dht11센서를 이용하여 습도 측정
      
      Serial.print("Temperature : ");
      Serial.println(Temp);
      Serial.print("Humidity : ");
      Serial.println(Humi); // 온도 및 습도를 시리얼 모니터를 통해 출력
      
      tv.setCursor(0, 0);
      tv.print("Temp : ");
      tv.print(Temp);

      tv.setCursor(0, 1);
      tv.print("Humi : ");
      tv.print(Humi); // lcd에 현재 온도 및 습도 기록(출력)

      delay(3000);
  
      Serial.println(tries);
      
      if(fire_state == 0){
        blink_LED();
        onFire();
      }
      else if(fire_state == 1){
        blink_LED();
        offFire();
        if(vib_state == 1){
          EarthquakeOccur();
        }
        else if(vib_state == 0){
          NoEarthquakeOccur();  
        }
      }
      //재난이 아닌 평상 시
      else{
         if(bluetooth.available()){
          char cmd = bluetooth.read();

          switch(cmd){
            case 'a':
              turnOffLights(); 
            case 'b':
              turnOnLights();
            case 'c':
              if(Temp >= Temp_C){
               turnOnAirConditioner(); 
              }
              else if(Temp < Temp_C){
               turnOffAirConditioner(); 
              }
            case 'd':
              openWindow();
            case 'e':
              closeWindow();
            case 'f':
              Smart_Door(angles);
            case 'g':
              if(distance_cm < DISTANCE_THRESHOLD){
                openBarricade();
              } else {
                closeBarricade();
              }
            case 'h':
              turnOnTv();
            case 'i':
              turnOffTv();
            case 'j':
              PlantGrawingSystemOn(WaterOfSoil, HumidOfSoil);
            case 'k':
              PlantGrawingSystemOff(red, green, blue);                  
          }
      }
  }


  //LED 불이 들어옴(5초간 깜빡거릴 것)
  //스피커 모듈을 통해 응급상황 발생 메시지(녹음 파일)출력 
  //--> 화재 발생: 화재 발생 했다는 녹음된 음성 출력
  //--> 지진 발생: 지진 발생했다는 녹음된 음성 출력 --> 측정 진도값 2개 설정 --> ex) 500이면 1번 음성, 600이상이면 2번 음성
  //응급상황: 화재 OR 지진
  //화재 발생시
  //--> 불꽃 감지 센서 작동 -> 화재가 발생했다는 lcd 문구 출력 -> 창문 열림
  //지진 발생시
  //--> 진동 센서 작동(진도 확인) -> 지진이 발생했다는 lcd 문구 출력 -> 문 열림
 
