void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // pc -> hm 10
  Serial.println("Hello World");
  Serial2.begin(9600); // hm10 -> pc
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(Serial.available() > 0){ // pc에 입력 (at command 입력)
    Serial2.write(Serial.read()); // 그 값을 hm10에 출력
  }
  if(Serial2.available() > 0){ // hm10에 입력(해당 문자 한 개를 통하여 모듈 제어 a, b, c, d ...)
    Serial.write(Serial2.read()); // hm10에 입력된 값을 시리얼 모니터를 통하여 제어
  }
}
