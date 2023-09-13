/*
   명지전문대학 정보통신공학과 2학년 RFID 프로젝트
   2018261042 장태민
   2018261043 최재혁
   2018261066 최하림
   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522       Arduino
               Reader/PCD    Mega
   Signal      Pin           Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST           5
   SPI SS      SDA(SS)       53
   SPI MOSI    MOSI          51
   SPI MISO    MISO          50
   SPI SCK     SCK           52

*/
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
#include <Adafruit_Sensor.h>

// RFID 상수 선언
#define SS_PIN 53
#define RST_PIN 5
// 초음파 상수 선언
#define TRIG 45
#define ECHO 47
//현관 불
#define R 9
#define G 10
#define B 11
//거실 불
#define R1 24
#define G1 25
#define B1 26
// 서브모터 상수 선언
#define motor 8
// 패시브 부저 상수 선언
#define buzzer 13

//변수 선언
int myRfid[4] = {169, 190, 250, 92}; // 출입할 수 있는 ID 넘버 설정
double duration = 0;
int distance = 10; //초음파가 감지하는 거리 설정
int angle = 0; //모터 시작 각도 0도로 설정

char secretCode[4] = {'0', '0', '0', '0'}; // 초기 비밀번호 1234로 설정
int tru = 0; //비밀번호 맞는지
int fal = 0; //비밀번호 틀린값
int count = 0; //비밀번호 입력한 횟수
int counta = 0;
int tru1 = 0;
int fal2 = 0;
int lcd_n = 10; // 비밀번호 입력 10번째 자리부터 lcd 표현
String user_pw = ""; // 등록할 비밀번호

//키패드의 행, 열의 갯수
const byte rows = 4;
const byte cols = 4;

//키패드의 버튼 위치 설정
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//키패드에 연결된 핀 번호 설정
byte rowPins[rows] = {37, 36, 35, 34};
byte colPins[cols] = {33, 32, 31, 30};

//객체 선언
Servo servo;
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;
LiquidCrystal_I2C lcd_1(0x27, 16, 2); // Lcd 설정 (주소는 0x27, 가로16, 세로2)
byte nuidPICC[4]; // 입력한 ID

void setup() {
  Serial.begin(9600);
  SPI.begin(); // SPI 실행
  rfid.PCD_Init(); // MRFID 실행

  //servo
  servo.attach(motor);
  servo.write(0);
  delay(500);
  servo.detach();

  //RGB
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  
  pinMode(R1, OUTPUT);
  pinMode(G1, OUTPUT);
  pinMode(B1, OUTPUT);

  //초음파
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);

  Serial2.begin(9600); // 핸드폰 화면

  //LCD 설정
  lcd_1.init();
  lcd_1.backlight();
  
  //buzzer
  pinMode(buzzer, OUTPUT);
  noTone(buzzer);
}


void loop() {
  btcheck(); //블루투스 연결
  noTone(buzzer);
  Serial.println(distanceCheck()); // 초음파 거리 측정
  tru = 0;
  count = 0;
  fal = 0;
  lcd_n = 10;
  servo.write(0);
  if ( distanceCheck() <= distance) { // 10cm 이내에 사람 접근
    servo.detach();
    print3Color('O'); // 황색 불빛 점등
    while (1) {

      btcheck();
      char key = keypad.getKey(); // 입력번호 받는 함수

      lcd_1.setCursor(0, 0);
      lcd_1.print("Password: ");
      print3Color('O');
      if (key) // 키패드 입력
      {
        Serial.println(key);
        if (key == '*') // '*' 입력 시 RFID로 전환
        {
          while (1) {
            btcheck();
            Serial.println("RFID 전환");
            lcd_1.setCursor(0, 0);
            lcd_1.print("RFID Change");
            lcd_1.setCursor(0, 1);
            lcd_1.print("Take a Card");
            delay(3000);
            lcd_1.clear();
            if ( ! rfid.PICC_IsNewCardPresent())
              break;

            // Verify if the NUID has been readed
            if ( ! rfid.PICC_ReadCardSerial())
              break;

            // 이전에 찍은 카드가 아닌 새로운 카드가 찍혔을 경우
            if (rfid.uid.uidByte[0] != nuidPICC[0] ||
                rfid.uid.uidByte[1] != nuidPICC[1] ||
                rfid.uid.uidByte[2] != nuidPICC[2] ||
                rfid.uid.uidByte[3] != nuidPICC[3] ) {
              Serial.println(F("A new card has been detected."));

              // 새로운 카드 ID를 다시 입력
              for (byte i = 0; i < 4; i++) {
                nuidPICC[i] = rfid.uid.uidByte[i];
              }

              Serial.println(F("The NUID tag is:"));
              Serial.print(F("In hex: "));
              printHex(rfid.uid.uidByte, rfid.uid.size);
              Serial.println();
              Serial.print(F("In dec: "));
              printDec(rfid.uid.uidByte, rfid.uid.size);
              Serial.println();
            }
            else {
              Serial.println(F("Card read previously.")); //이전에 찍은 카드가 찍혔을 경우
              lcd_1.clear();
              lcd_1.setCursor(0, 0);
              lcd_1.print("Card read prev");
              delay(300);
              lcd_1.clear();
            }
            // Halt PICC
            rfid.PICC_HaltA();

            // 카드 ID가 맞을 경우
            if ((nuidPICC[0] == myRfid[0] && nuidPICC[1] == myRfid[1] && nuidPICC[2] == myRfid[2] && nuidPICC[3] == myRfid[3])) {
              for (byte i = 0; i < 4; i++) {
                nuidPICC[i] = 0;
              }

              Serial.println("Open");

              Su();
              delay(500);
              break;
            } else { // 카드 ID가 틀릴 경우
             
              Fa();

            }
            break;
          }
          // 'A'가 눌릴 경우 비밀번호 변경
        } else if ( key == 'A') {
          user_pw = "";
          Serial.println("비밀번호 변경");
          lcd_1.setCursor(0, 0);
          lcd_1.print("Password Change");
          while (1) {
            key = keypad.waitForKey();
            if (key >= '0' && key <= '9') {
              if (user_pw.length() < 4) {
                //비밀번호 길이를 4개로 맞추기 위해서
                user_pw = user_pw + key;

              }
              String star = "";
              for (int i = 0; i < user_pw.length(); i++) {
                //현재 입력되어있는 비밀번호 갯수만큼 루프가 회전
                star += '*';
                lcd_1.setCursor(0, 1);
                lcd_1.print(star);

              }
              delay(300);
            }
            Serial.print(user_pw);
            Serial.print("/");
            Serial.println(user_pw.length());
            if (user_pw.length() == 4) {
              lcd_1.clear();
              lcd_1.setCursor(0, 0);
              lcd_1.print("Successful change.");
              delay(1000);
              lcd_1.clear();
              break;
              /*
                if ( key == user_pw[counta] ) {
                counta++;
                tru1++;
                } else if (key != user_pw[counta]) {
                counta++;
                fal2++;
                } if (counta == 4) {
                if (tru1 == 4) {
                  Serial.println("문이 열립니다");
                  break;
                }
                else {
                  Serial.println("비밀번호가 옳지 않습니다.");
                }
                counta = 0;
                tru1 = 0;
                fal2 = 0;
                }
              */
            }
          }

        }
        else {
          if ((key == user_pw[count]) || (key == secretCode[count])) //비밀번호와 입력번호 맞을시
          {
            lcd_1.setCursor(lcd_n, 0);
            lcd_1.print(key);
            count++;
            tru++;
            lcd_n++;
            tone(buzzer, 523);
            delay(100);
            noTone(buzzer);
          }
          else if (key != user_pw[count]) //비밀번호와 입력번호 틀릴시
          {
            lcd_1.setCursor(lcd_n, 0);
            lcd_1.print(key);
            count++;
            fal++;
            lcd_n++;
            tone(buzzer, 523);
            delay(100);
            noTone(buzzer);
          }
          if (key == '#') //초기화 버튼
          {
            re(); //초기화 함수
          }
          if (count == 4) //count개수가 비밀번호 개수와 같을시
          {
            if (tru == 4)
            {
              delay(500);
              Su(); //성공 함수

              break;
            }
            else
            {
              delay(500);
              Fa(); // 실패 함수
            }
            tru = 0;
            count = 0;
            fal = 0;
            lcd_n = 10;
          }
        }
      }
    }
  }
}



void print3Color(char color) { // 현관문 전등
  switch (color) {
    case 'R':
      digitalWrite(R, HIGH);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      break;

    case 'G':
      digitalWrite(R, LOW);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
      break;

    case 'B':
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, HIGH);
      break;

    case 'O':
      digitalWrite(R, HIGH);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
      break;

    case 'N':
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      break;

    case 'W':
      digitalWrite(R, HIGH);
      digitalWrite(G, HIGH);
      digitalWrite(B, HIGH);
      break;
  }
}

void p3Color(char color) { // 거실 전등
  switch (color) {
    case 'R':
      digitalWrite(R1, HIGH);
      digitalWrite(G1, LOW);
      digitalWrite(B1, LOW);
      break;

    case 'G':
      digitalWrite(R1, LOW);
      digitalWrite(G1, HIGH);
      digitalWrite(B1, LOW);
      break;

    case 'B':
      digitalWrite(R1, LOW);
      digitalWrite(G1, LOW);
      digitalWrite(B1, HIGH);
      break;

    case 'N':
      digitalWrite(R1, LOW);
      digitalWrite(G1, LOW);
      digitalWrite(B1, LOW);
      break;

    case 'W':
      digitalWrite(R1, HIGH);
      digitalWrite(G1, HIGH);
      digitalWrite(B1, HIGH);
      break;
  }
}

// 문 동작
void door() { 
  btcheck();
  servo.attach(motor);
  for (angle = 0; angle < 80; angle ++)
  {
    servo.write(angle);
    delay(10);
  }
  delay(5000);
  for (angle = 80; angle > 0; angle --)
  {
    servo.write(angle);
    delay(10);
  }
}

// 성공했을 때 동작
void Su()
{
  btcheck();
  Serial.println("the door open.");
  tone(buzzer, 659);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 1175);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 2093);
  delay(200);
  noTone(buzzer);
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("The door open.");
  p3Color('W');
  print3Color('N');
  door();

  lcd_1.clear();
  servo.detach();
}

// 실패 했을 때 동작
void Fa()
{
  btcheck();
  tone(buzzer, 2093);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 2093);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 2093);
  delay(200);
  noTone(buzzer);
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Password wrong.");
  Serial.println("fail");
  delay(1000);
  lcd_1.clear();
}

// 리셋 할 때 동작
void re()
{
  btcheck();
  tru = 0;
  count = 0;
  fal = 0;
  lcd_n = 10;
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Password reset.");
  Serial.println("password reset");
  delay(1000);
  lcd_1.clear();
}

// 거리 측정
int distanceCheck() {
  btcheck();
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  return ((float)(340 * duration) / 10000) / 2;
}


// 블루투스 동작
void btcheck() {
  if (Serial.available()) {    // 시리얼 모니터로부터 데이터가 들어온 경우 수행 루틴
    serifunc();
  }

  if (Serial2.available()) {   // HC 모듈로부터 데이터가 들어온 경우 수행 루틴
    seri2func();
  }

  return;
}

void serifunc() {              // Serial2 모듈로부터 들어온 데이터를 시리얼 모니터로 출력
  Serial2.write(Serial.read());
}

void seri2func() {             // 시리얼 모니터로부터 들어온 데이터를 Serial2로 출력
  char z = Serial2.read();
  if (z == 'a') { //  시리얼 모니터에 a가 입력될 시 led 켜짐
    digitalWrite(R1, HIGH);
    digitalWrite(G1, HIGH);
    digitalWrite(B1, HIGH);
  }
  else if (z == 'b') { // 시리얼 모니터에 b가 입력될 시 led 꺼짐
    digitalWrite(R1, LOW);
    digitalWrite(G1, LOW);
    digitalWrite(B1, LOW);
  }
  else if (z == 'c') { // 시리얼 모니터에 c가 입력될 시 문이 열리는 open()함수 실행
    open();
  }
  Serial.write(z);

}

// 블루투스로 문 열때
void open() {
  servo.attach(motor);
  for (angle = 0; angle < 80; angle ++)
  {
    servo.write(angle);
    delay(10);
  }
  p3Color('W');
  print3Color('N');
  delay(3000);
  close();
  delay(7000);

}

void close() {
  for (angle = 80; angle >= 1; angle -= 1)
  {
    servo.write(angle);
    delay(10);
  }
}

// RFID ID인식 hex 와 dec 타입
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
