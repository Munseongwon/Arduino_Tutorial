#include <OneWire.h> //온도센서 라이브러리
 int fire_sensor = 4; //불꽃 감지 센서 4번핀으로
 int DS18S20 = 2; //온도센서 2번핀으로
 //int led = 7; //led 7번핀으로
 //int speakerpin = 5; //스피커 5번핀으로
 OneWire ds(DS18S20);

 void setup() {
   pinMode(fire_sensor, INPUT);
   //pinMode(led, OUTPUT); //입력, 출력 설정
   //digitalWrite(led, LOW);
   Serial.begin(9600); //시리얼 모니터 설정
 }

 void loop() {
   int value = digitalRead(fire_sensor); //불꽃 감지 센서 값 저장
   float temperature = getTemp(); //온도센서 함수 불러오기

   if (value==0 && temperature>=27) { //불꽃이 감지되고, 온도가 27도 이상일 때
     Serial.write('1'); //시리얼 통신에 '1' 전송(블루투스)
     //digitalWrite(led, HIGH);
     //tone(speakerpin, 1319, 150);
     delay(1000);
     //digitalWrite(led, LOW);
     //noTone(speakerpin);
     //delay(100); //led, 스피커 작동
   }
   else { //불꽃이 감지되지 않거나 온도가 27도 이하일 때
     Serial.write('2'); //시리얼 통신에 '2' 전송(블루투스)
     //digitalWrite(led, LOW);
     //noTone(speakerpin); //led, 스피커 작동 중지
   }
   delay(5000); //앱에서의 통신 속도와 맞추기 위한 딜레이
 }

 float getTemp() { //온도센서 함수
   byte data[12];
   byte addr[8];
   if (!ds.search(addr)) {
     ds.reset_search();
     return -1000;
   }

   if ( OneWire::crc8( addr, 7) != addr[7]) {
     return -1000;
   }
   if ( addr[0] != 0x10 && addr[0] != 0x28) {
     return -1000;
   }

   ds.reset();
   ds.select(addr);
   ds.write(0x44,1);

   byte present = ds.reset();
   ds.select(addr);   
   ds.write(0xBE);
   
   for (int i = 0; i < 9; i++) {
     data[i] = ds.read();
   }
   
   ds.reset_search();
   
   byte MSB = data[1];
   byte LSB = data[0];

   float tempRead = ((MSB << 8) | LSB);
   float TemperatureSum = tempRead / 16;
   
   return TemperatureSum;
 }
