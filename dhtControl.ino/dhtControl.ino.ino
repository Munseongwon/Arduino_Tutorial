#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     //DATA Pin 
#define DHTTYPE DHT11 //DHT22 라고 TYPE을 정의함

#define LEDPIN 3 //LED PIN
//알람 설정 온도
//26도
#define THRESHOLD_VAL 26.0f 
       
//dht 클랙서 생성 (Data Pin, Sensor Type)
DHT_Unified dht(DHTPIN, DHTTYPE);

//대기 시간 변수
uint32_t delayMS;

void setup() {
  //모니터 프로그램을 위한 시리얼 시작
  Serial.begin(9600); 

  //LED PIN 출력으로 설정
  pinMode(LEDPIN, OUTPUT);

  //DHT11 시작
  dht.begin();
  
  sensor_t sensor;
  dht.humidity().getSensor(&sensor);

  //최소한의 대기시간을 얻어 온다.
  delayMS = sensor.min_delay / 1000;  
}

void loop() {
  //대기
  delay(delayMS);
  
  sensors_event_t event;  
  //온도값 얻기
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    //얻기 실패했다면...
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    //실제 온도 값
    Serial.print(event.temperature);
    Serial.println(" *C");
    if(event.temperature <= THRESHOLD_VAL){      
      //위에서 지정한 THRESHOLD_VAL 값 이하이면 LED가 켜진다.
      digitalWrite(LEDPIN, HIGH);
    }else{      
      //값보다 온도가 올라가면 LED는 꺼진다.
      digitalWrite(LEDPIN, LOW);
    }
  }
}
