#include <Servo.h>
#include <LiquidCrystal_I2C.h>

Servo garservo; // 주차장의 문을 열도록 하는 주차장 문 서보모터 객체 생성 
LiquidCrystal_I2C Monitor(0x27, 16, 2); //lcd 모니터 객체 생성

const int TRIG_PIN = 6;
const int ECHO_PIN = 7;
const int SERVO_PIN = 9;
const int DISTANCE_THRESHOLD = 30; // 물체의 임계 거리

float duration_us, distance_cm;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 9600초로 설정
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  garservo.attach(SERVO_PIN); // 9번핀에 주차장 서보모터 연결
  garservo.write(0); // 0도로 초기화 함 --> 스마트 주차장 초기 각도

  Monitor.init();
  Monitor.backlight();
}


void loop() {
  // put your main code here, to run repeatedly:
  
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, HIGH);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.02 * duration_us;

   if(distance_cm < DISTANCE_THRESHOLD){
    garservo.write(90); // 측정거리가 임계거리보다 작을 때 서보모터 90도로 읽음
    Monitor.clear(); // 모니터에 남아있는 글자가 있으면 모두 삭제
    Monitor.print("Open"); // 모니터에 열려 있다는 것을 구현
    delay(1000); // 1초간의 딜레이를 줌
   }
   else{
    garservo.write(0);  // 측정거리가 임계거리보다 클 시에는 서보모터 0도로 읽음
    Monitor.clear(); 
    Monitor.print("Close");
    delay(2500); //2.5초간의 딜레이를 줌
   }
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    delay(100); 
   // 시리얼 모니터를 통해 현재 초음파센서가 측정하고 있는 거리가 어느 정도인지 출력, 
   이 때 시리얼모니터가 거리를 측정하고 출력하는 속도는 0.1초로 설정함
}
