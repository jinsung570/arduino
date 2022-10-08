#include <SimpleDHT.h> // 온습도센서 라이브러리
#include <LiquidCrystal.h> //LCD 라이브러리

#define DHTPIN 7 //온습도 센서 아두이도 연결핀 번호
#define DHTTYPE DHT11 //DHT 타입 정하기 -DHT11
#define LCD_resolution 6 //LCD 선명도 연결핀
SimpleDHT11 dht11(DHTPIN); //DHT 설정 (온습도 센서 설정)
LiquidCrystal lcd(13,12,5,4,3,2); //LCD 핀 설정

#define Dust_LED_PIN 11 //미세먼지 센서 LED핀
#define DUST_OUT_PIN A0 //미세먼지 센서값 출력핀
int samplingTime=280;
int deltaTime=40;
int sleepTime=9680;
float dust_sensorValue=0;
float voltage=0;
float dust_density=0;

#define RED 8
#define GREEN 9
#define BLUE 10

void setup(){ //초기화 
  Serial.begin(9600);
  lcd.begin(16,2); //LCD 사용 설정
  analogWrite(LCD_resolution,100); //LCD 글자 선명도
  lcd.clear();
  pinMode(Dust_LED_PIN,OUTPUT); //먼지센서 LED 출력모드
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
}

void loop(){ //메인 코드
  dht_check(); //온습도 체크->출력
  dust_check(); //미세먼지 체크->출력
  RGB_LED_check(); //RGB LED 체크 (미세먼지 농도에 따라 색깔 출력)
}

void dht_check(){ //온습도를 확인하여 출력하는 코드
  delay(2000); //센서가 작동되기까지 기다림
  byte t=0; //온도 변수
  byte h=0; //습도 변수
  dht11.read(&t,&h,NULL); //온습도 값 읽어들임

  lcd.setCursor(0,0); //LCD 좌측 위 첫칸
  lcd.print("T: ");
  lcd.print(t); //온도값 출력
  lcd.print(" C");
  lcd.setCursor(9,0); //LCD 첫줄 중앙 쯤
  lcd.print("H: ");
  lcd.print(h);
  lcd.print(" %");
}

void dust_check(){ //미세먼지 측정, 출력 코드
  digitalWrite(Dust_LED_PIN,LOW); //적외선 LED On
  delayMicroseconds(samplingTime); //280us=0.28ms(딜레이)
  dust_sensorValue=analogRead(DUST_OUT_PIN); //먼지센서 출력값 읽기
  delayMicroseconds(deltaTime); //for pulse width of 0.32ms=0.28ms+0.04ms
  digitalWrite(Dust_LED_PIN,HIGH); //적외선 LED off
  delayMicroseconds(sleepTime); //9.680ms (10ms=9.680ms+0.32ms)
  voltage=dust_sensorValue * (5.0/1023.0); //센서값을 0~5v로 변환하기
  dust_density=50*voltage;  //ug/m3로 단위 변환 -> 미세먼지 단위
  lcd.setCursor(0,1);
  lcd.print("Dust: ");
  lcd.print(dust_density);
  lcd.print(" ug/m3"); //pm 2.5 단위
}

void RGB_LED_check(){ //by PM 2.5
  if(dust_density>=151)LED_Color(1,0,0); //빨간색
  else if(dust_density>=51)LED_Color(1,1,0); //노란색
  else if(dust_density>=16)LED_Color(0,1,0); //초록색
  else if(dust_density>=0)LED_Color(0,0,1); //파란색
}

void LED_Color(int r,int g,int b){
  digitalWrite(RED,r);
  digitalWrite(GREEN,g);
  digitalWrite(BLUE, b);
}
