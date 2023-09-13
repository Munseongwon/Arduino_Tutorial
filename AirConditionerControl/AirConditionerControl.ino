#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const int pinDHT = 3;
const int pinAirCondtioner = 7; // 에어컨 led핀 7번으로 정의
const int pinPiezo = 2;
DHT_Unified dht(3, DHT11);

int notes[] = {261, 294, 330, 349, 392, 440, 494, 523};

float humidity = 0.; // 습도 초기화 
float temperature = 0.; // 온도 초기화 
float discomfortIndex = 0; // 불쾌지수 초기화

boolean isAirConditionerTurnOn = false;

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  Serial.begin(9600);  
}

//에어컨을 켜는 함수
void turnOnAirConditioner(){
  digitalWrite(pinAirConditioner, HIGH);
  //에어컨이 켜져있는 상태일 시 음악 재생
  if(!isAirConditionerTurnOn){
    for(int i=0; i<8; i++){
      tone(pinPiezo, notes[i], 300);
      delay(100);
    }
    isAirConditionerTurnOn = true;
  }
}

//에어컨을 끄는 함수
void turnOffAirConditioner(){
  digitalWrite(pinAirConditioner, LOW);
  isAirConditionerTurnOn = false;
}
void loop() {
  // put your main code here, to run repeatedly:
  sensor_event_t event;
  dht.temperature().getEvent(&event);
  humidity = event.relative_humidity;
  temperature = event.temperature;

  //불쾌지수 계산
  discomfortIndex = (1.8 * temperature) - (0.55 * (1 - humidity / 100.0) * (1.8 * temperature - 26)) + 32;

  //불쾌지수 70이상이면 에어컨 작동
  if(discomfortIndex >= 70){
    turnOnAirConditioner();
  }else{
    turnOffAirConditioner();
  }
}
