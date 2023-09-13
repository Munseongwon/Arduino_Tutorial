#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


#define DHTPIN 2     //DATA Pin 
#define DHTTYPE DHT11 //DHT11 라고 TYPE을 정의함

#define LEDPIN  3 //LED PIN
//알람 설정 온도
//26도
#define Temp_C  26.0

//dht 클랙서 생성 (Data Pin, Sensor Type)
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial MP3Module(7,8);
DFRobotDFPlayerMini MP3Player;

int tries; // 에어컨이 동작한 횟수


void setup() {
  
  Serial.begin(9600); //모니터 프로그램을 위한 시리얼 시작
  pinMode(LEDPIN, OUTPUT); //LED PIN 출력으로 설정
  dht.begin(); //DHT11 시작
  lcd.init(); // lcd 초기화
  lcd.backlight(); // lcd가 켜졌다 꺼졌다 함
  MP3Module.begin(9600);
  if (!MP3Player.begin(MP3Module,false)) { // MP3 모듈을 초기화합니다. 초기화에 실패하면 오류를 발생시킵니다.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  MP3Player.volume(15); // 음성 볼륨의 크기는 15로 설정

}

void loop() {

float Temp = dht.readTemperature(); // dht11센서를 이용하여 온도 측정
float Humi = dht.readHumidity(); // dht11센서를 이용하여 습도 측정

Serial.print("Temperature : ");
Serial.println(Temp);
Serial.print("Humidity : ");
Serial.println(Humi); // 온도 및 습도를 시리얼 모니터를 통해 출력

  lcd.setCursor(0, 0);
  lcd.print("Temp : ");
  lcd.print(Temp);

  lcd.setCursor(0, 1);
  lcd.print("Humi : ");
  lcd.print(Humi); // lcd에 현재 온도 및 습도 기록(출력)

  delay(3000);
  
  Serial.println(tries);
  
  if(Temp >= Temp_C){  
      //현재온도가 Temp_C 값 이상이면 LED(에어컨)가 켜진다.
      digitalWrite(LEDPIN, HIGH); //에어컨이 동작할 시 led 켜짐
      if(tries <1){ // 에어컨이 처음 켜진 것이라면
      MP3Player.play(1); // 1번 음악(동작 재생 음악)
      tries++;
      }
  } else if(Temp < Temp_C){     
      //현재온도가 Temp_C 값 이상이면 LED(에어컨)가 켜진다.
      digitalWrite(LEDPIN, LOW); // 에어컨이 꺼질 시 led 꺼짐
      if(tries >=1){ // 에어컨이 켜진 상태(즉 1번 이상이라도 켜져 있다면)
      MP3Player.play(2); // 2번 음악 재생(동작 중지 음악)
      }
      tries = 0; // 다시 원래대로 꺼져 있는 상태로 되돌아옴
  }
}
