#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int led[] = {5,6,7};

void setup() {
  // put your setup code here, to run once:
  lcd.init();  //lcd 초기화
  lcd.backlight(); // lcd 배경 불 켜기
  Serial.begin(9600);

  dht.begin(); //dht 11센서 작동
  for(int i=1; i<=3; i++){
    pinMode(led[i], OUTPUT);
  }
}

//물의 양 적정하거나 온도와 습도 역시 적정할 때, green_led 켜짐(1초간 깜빡거림 설정)
void Enough() {
  lcd.clear();
  lcd.backlight();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Enough");
  delay(1000);
  digitalWrite(led[2], HIGH);
  delay(1000);
  digitalWrite(led[2], LOW);
  delay(1000);
}


//물의 양 또는 빛의 양이 너무 많고, 온도와 습도가 너무 높을 때, blue_led 켜짐(1초간 깜빡거림 설정)
void High() {
  lcd.clear();
  lcd.backlight();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Many");
  delay(1000);
  digitalWrite(led[3], HIGH);
  delay(1000);
  digitalWrite(led[3], LOW);
  delay(1000);
}


// 물의 양 또는 빛의 양이 너무 적거나, 온도와 습도가 너무 낮을 때, red_led 켜짐(1초간 깜빡거림 설정)
void Low() {
  lcd.clear();
  lcd.backlight();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Little");
  delay(1000);
  digitalWrite(led[1], HIGH);
  delay(1000);
  digitalWrite(led[1], LOW);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  int waterOfSoil = analogRead(A1);  // 토양의 수분의 양 변수값을 a1핀을 통해 읽어들인다(0~1023)
  int humidityOfIndoor = dht.readHumidity();  //토양의 습도 값을 dht11센서로부터 h변수에 저장
  int tempOfIndoor = dht.readTemperature(); //실내의 온도 값을 dht11센서로부터 t변수에 저장

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
