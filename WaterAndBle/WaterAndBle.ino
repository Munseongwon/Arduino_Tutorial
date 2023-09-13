//아두이노 메가보드 2
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

#define red 26 // red led 5번핀 설정
#define green 27 // green led 6번핀 설정
#define blue 28 // blue led 7번핀 설정

Servo servo_win, servo_main;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int main_PIN = 10;
const int win_PIN = 11;

char data;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  lcd.init();
  lcd.backlight();
  servo_main.attach(main_PIN);
  servo_main.write(0);
  servo_win.attach(win_PIN);
  servo_win.write(0);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial1.available()){
    data = Serial1.read();
    if(data == '1'){
      servo_main.write(90);
    }else if(data == '2'){
      servo_main.write(0);
    }else if(data == '3'){
      servo_win.write(90);
    }else if(data == '4'){
      servo_win.write(0);
    }
  }
int waterOfSoil = analogRead(A1);// 토양의 수분의 양 변수값을 a1핀을 통해 읽어들인다(0~1023)
if((waterOfSoil>=200 && waterOfSoil<499)){
Enough();
Serial.println(waterOfSoil);
}
else if((waterOfSoil<200)){
Low();
Serial.println(waterOfSoil);
}
else if((waterOfSoil>=499)){
High();
Serial.println(waterOfSoil);
}
}
//물의 양 적정할 때, green_led 켜짐(1초간 깜빡거림 설정)
void Enough() {
lcd.clear();
lcd.backlight();
lcd.display();
lcd.setCursor(0, 0);
lcd.print("Enough");
delay(1000);
digitalWrite(green, HIGH);
delay(1000);
digitalWrite(green, LOW);
delay(1000);
}
//물의 양이 너무 많을 때, blue_led 켜짐(1초간 깜빡거림 설정)
void High() {
lcd.clear();
lcd.backlight();
lcd.display();
lcd.setCursor(0, 0);
lcd.print("Many");
delay(1000);
digitalWrite(blue, HIGH);
delay(1000);
digitalWrite(blue, LOW);
delay(1000);
}
// 물의 양이 너무 적을 때, red_led 켜짐(1초간 깜빡거림 설정)
void Low() {
lcd.clear();
lcd.backlight();
lcd.display();
lcd.setCursor(0, 0);
lcd.print("Little");
delay(1000);
digitalWrite(red, HIGH);
delay(1000);
digitalWrite(red, LOW);
delay(1000);
}
