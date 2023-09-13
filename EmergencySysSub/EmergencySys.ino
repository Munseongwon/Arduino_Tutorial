#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Servo.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


#define DHTPIN    2     //DATA Pin 
#define DHTTYPE   DHT11 //DHT11 라고 TYPE을 정의함
#define LEDPIN    3 //LED PIN
#define Temp_C    26.0
#define red 26 // red led 5번핀 설정
#define green 27 // green led 6번핀 설정
#define blue 28 // blue led 7번핀 설정

DHT dht(DHTPIN, DHTTYPE);//dht 클랙서 생성 (Data Pin, Sensor Type)
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial MP3Module(52, 53); //MP3(Rx,Tx)
Servo servo_gar;
DFRobotDFPlayerMini MP3Player;

//4번 8번 xx
const int gar_PIN = 5;
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;
int fire = 11;
int vibr = 13;
int led_R = 22;
int led_G = 23;
int led_B = 24;

int tries1, tries2 ;
int pos;
int val_fire = 0;
float duration_us, distance_cm;
const int DISTANCE_THRESHOLD = 100;

void setup() {
  Serial.begin(9600);//모니터 프로그램을 위한 시리얼 시작
  Serial1.begin(9600);
  MP3Module.begin(9600);
  dht.begin(); //DHT11 시작
  pinMode(LEDPIN, OUTPUT); //LED PIN 출력으로 설정
  pinMode(led_R, OUTPUT);
  pinMode(led_G, OUTPUT);
  pinMode(led_B, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(fire, INPUT);
  pinMode(vibr, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  servo_gar.attach(gar_PIN);
  servo_gar.write(0); // 0도로 초기화 함 --> 스마트 주차장 초기 각도
  lcd.init();
  lcd.backlight();
  if (!MP3Player.begin(MP3Module, false)) { // MP3 모듈을 초기화합니다. 초기화에 실패하면 오류를 발생시킵니다.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  MP3Player.volume(15);
}

void loop() {
  digitalWrite(led_R, LOW);
  digitalWrite(led_G, LOW);
  digitalWrite(led_B, LOW);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, HIGH);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = ((duration_us * 340) / 10000) / 2;
  delay(500);

  float Temp = dht.readTemperature();
  float Humi = dht.readHumidity();
  int val_fire = digitalRead(fire);
  int val_vib = digitalRead(vibr);


  Serial.print("Temperature : ");
  Serial.println(Temp);
  //Serial.println(*);
  Serial.print("Humidity : ");
  Serial.println(Humi);
  //Serial.println(*);
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  Serial.print("Fire = ");
  Serial.println(val_fire);
  Serial.print("Vibr = ");
  Serial.println(val_vib);
  //delay(100);

    //lcd.clear();
    /*lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(Temp);
    lcd.setCursor(0, 1);
    lcd.print("Humi : ");
    lcd.print(Humi);*/
    

  //  Serial.println(tries);

  //에어컨 작동
  if (Temp >= Temp_C) {
    //현재온도가 Temp_C 값 이상이면 LED(에어컨)가 켜진다.
    digitalWrite(LEDPIN, HIGH);
    if (tries1 < 1) {
      MP3Player.play(1);
      tries1++;
    }
  } else if (Temp < Temp_C) {
    //현재온도가 Temp_C 값 이상이면 LED(에어컨)가 켜진다.
    digitalWrite(LEDPIN, LOW);
    if (tries1 >= 1) {
      MP3Player.play(2);
    }
    tries1 = 0;
  }

  // 차량 입출차
  if (0 < distance_cm && distance_cm < DISTANCE_THRESHOLD) {
    servo_gar.write(90); // 측정거리가 임계거리보다 작을 때 서보모터 90도로 읽음
    lcd.clear(); // 모니터에 남아있는 글자가 있으면 모두 삭제
    if (tries2 < 1) {
      MP3Player.play(3);
      lcd.print("Open"); // 모니터에 열려 있다는 것을 구현
      delay(2000);
      tries2++;
    }
    //delay(500); // 1초간의 딜레이를 줌
  }
  else {
    servo_gar.write(0);  // 측정거리가 임계거리보다 클 시에는 서보모터 0도로 읽음
    lcd.clear();
    if (tries2 >= 1) {
      MP3Player.play(4);
      lcd.print("Close");
      delay(2000);
    }
    tries2 = 0;
    //delay(500); //2.5초간의 딜레이를 줌
  }

  if (val_fire == 0) {
    lcd.clear();
    digitalWrite(led_R, HIGH);    //불꽃 감지 시
    //servo_win.write(90);
    Serial1.write('1');
    //delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("!!!!WARNINGS!!!!");
    lcd.setCursor(0, 1);
    lcd.print(" !FIRE DETECTED!");
    fire_sound();
  } else if (val_vib == 1) {
    lcd.clear();
    digitalWrite(led_R, HIGH);
    //servo_main.write(90);
    Serial1.write('2');
    lcd.setCursor(0, 0);
    lcd.print("!!!!WARNINGS!!!!");
    lcd.setCursor(0, 1);
    lcd.print("!!!EARTHQUAKE!!!");
    //delay(1000);
    earth_sound();
  } else  {
    Serial1.write('3');
    lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(Temp);
    lcd.setCursor(0, 1);
    lcd.print("Humi : ");
    lcd.print(Humi);
  }

}

void earth_sound(){
  MP3Player.play(6);
    delay(2000);
}

void fire_sound(){
  MP3Player.play(5);
    delay(2000);
}
