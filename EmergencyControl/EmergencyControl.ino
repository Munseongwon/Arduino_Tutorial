#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C tv(0x3F, 16, 2);
Servo mainsv; // 현관
Servo windsv; // 창문

const int vibr = 2;
const int fire = 5;
const int led_R = 3;
const int speaker = 10;
const int door = 11;
const int window = 12;

int pos1 = 0; // 현관 서보모터 각도 초기화
int pos2 = 0; // 창문 서보모터 각도 초기화
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  
  mainsv.attach(11);
  mainsv.write(0);
  windsv.attach(12);
  windsv.write(0);
  
  pinMode(vibr, INPUT);
  pinMode(fire, INPUT);

  pinMode(led_R, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led_R, LOW);

  int val_vibr = digitalWrite(vibr);
  int val_fire = digitalWrite(fire);

  if(val_vibr == 1){ // 지진 발생한 경우
    digitalWrite(led_R, HIGH);
    lcd.setCursor(1,0);
    lcd.print("!!!!WARNINGS!!!!");
    lcd.setCursor(1,1);
    lcd.print("EARTHQUAKE!");

    for(pos1= 0; pos1<=90; pos1+=30){
      mainsv.write(pos1);
    }

    for(int i=262; i<=523; i++){
      tone(speaker, i);
      delay(10);
    }

    for(int i=523; i>=262; i--){
      tone(speaker, i);
      delay(10);
    }
    noTone(speaker);
  }

  if(val_fire == 0){ // 화재 발생한 경우
    digitalWrite(led_R, HIGH);
    lcd.setCursor(1,0);
    lcd.print("!!!!WARNINGS!!!!");
    lcd.setCursor(2,1);
    lcd.print("FIRE DETECTED!");

    for(pos2= 0; pos2<=90; pos2+=30){
      windsv.write(pos1);
    }

    for(int i=262; i<=523; i++){
      tone(speaker, i);
      delay(10);
    }

    for(int i=523; i>=262; i--){
      tone(speaker, i);
      delay(10);
    }
    noTone(speaker);
  }
  Serial.print("Vibration = ");
  Serial.println(val_vib); 
  Serial.print("Fire = "); 
  Serial.println(val_fire); 
  delay(1000);
}
