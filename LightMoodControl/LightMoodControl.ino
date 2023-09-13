void setup() {
  // put your setup code here, to run once:
  for(int i=2; i<=7; i++){
    pinMode(i, OUTPUT);
  }
}

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


void loop() {
  // put your main code here, to run repeatedly:
  turnOnLights();
  delay(2500);
  turnOffLights(); 
}
