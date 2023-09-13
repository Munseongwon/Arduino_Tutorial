#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

const int pinDHT = 4;
const int pinPiezo = 2;
//const int pinAirConditioner = 6;
DHT_Unified dht(4, DHT11);

LiquidCrystal_I2C TV(0x27, 16, 2);
Servo window;
// SoftwareSerial bluetooth(4,5);

//int notes[] = {261, 294, 330, 349, 392, 440, 494, 523};

float humidity = 0.; // 습도의 기본값입니다.
float temperature = 0.; //온도의 기본값입니다.
float discomfortIndex = 0.; //불쾌지수의 기본값입니다.

boolean isTVTurnOn = false; // tv가 켜졌는지 꺼져있는지 확인
 
boolean isFireOn = false; // 화재 발생했는지 아니면 평상시인지
boolean isEearthquake = false; // 지진 발생했는지 아니면 평상시인지

boolean isTVModePrintFire = false; // tv에 화재 발생했는지
boolean isTVModePrintEarthquake = false; // tv에 지진 발생했는지
boolean isTVModePrintTemperature = false; // tv에 현재 온도,습도 출력해야하는지


void setup() {
  // put your setup code here, to run once:
  //pinMode(pinPiezo, OUTPUT);
  window.attach(7); // 창문 서보모터를 7번핀에 꽂음
  dht.begin();
  
  TV.init();
  Serial.begin(9600);
  bluetooth.begin(9600);
}

void turnOnTV(String content = "Fire!"){
  TV.setCursor(0,0);
  TV.backlight();
  TV.print(content);
  isTVTurnOn = true;
}

void turnOnTV(String content = "EearthQuake"){
  TV.setCursor(0,0);
  TV.backlight();
  TV.print(content);
  isTVTurnOn = true;
}

void turnOnTV(String temp, String humi){
  TV.setCursor(0, 0);
  TV.backlight();
  TV.print(temp); // TV에 현재 온도값 출력
  TV.setCursor(0, 1);
  TV.print(humi); // TV에 현재 습도값 출력
  isTVTurnOn = true;
}

void turnOffTV(){
  TV.clear();
  TV.noBacklight();
  isTVTurnOn = false;
}

void WindowOpen(){
  
}

//에어컨을 켜는 함수
/*void turnOnAirConditioner(){
  digitalWrite(pinAirConditioner, HIGH);
  //에어컨이 꺼져있는 상태일 시 음악 재생
  if(!isAirConditionerTurnOn){
    for(int i=0; i<8; i++){
      tone(pinPiezo, notes[i], 300);
      delay(100);
    }
    isAirConditionerTurnOn = true;
  }
}*/

void loop() {
  // put your main code here, to run repeatedly:
  
  //온습도 센서로 온도와 습도 측정
  /*sensors_event_t event;
  dht.temperature().getEvent(&event);
  humidity = event.relative_humidity;
  temperature = event.temperature;*/
  
  // 불쾌지수 계산
  //discomfortIndex = (1.8 * temperature) - (0.55 * (1 - humidity / 100.0) * (1.8 * temperature - 26)) + 32;

  //불쾌지수가 70 이상이면 에어컨 작동
  /*if(discomfortIndex >= 70){
    turnOnAirConditioner();
  }else{
    turnOffAirConditioner();
  }*/

  
}
