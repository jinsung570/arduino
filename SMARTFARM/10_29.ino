#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define A_SOIL_HUMI A0
#define PUMP_A 6
#define PUMP_B 5
#define RGB_R 10
#define RGB_G 9
#define RGB_B 11
#define BT_TX 7
#define BT_RX 8
#define On_Time 4000

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial bt(BT_TX, BT_RX);

unsigned long pm=0;
int soilHumidity;
void initPin() {
  Serial.begin(9600);
  bt.begin(9600);
  lcd.init();
  pinMode(PUMP_A,OUTPUT);
  pinMode(PUMP_B,OUTPUT);
  pinMode(RGB_R,OUTPUT);
  pinMode(RGB_G,OUTPUT);
  pinMode(RGB_B,OUTPUT);

  digitalWrite(RGB_R,LOW);
  digitalWrite(RGB_G,LOW);
  digitalWrite(RGB_B,LOW);
  analogWrite(PUMP_A,0);
  analogWrite(PUMP_B,0);

}

void introLcd(){
  lcd.print("SMART FARM");
  lcd.setCursor(0,1);
}

void printLcd(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Moisture: ");
  lcd.print(" ");
  lcd.setCursor(11,0);
  lcd.print(soilHumidity);
  lcd.print("%");
}

void initLcd(){
  lcd.backlight();
  lcd.setCursor(0,0);
  introLcd();
}

void calcSoilHumidity(){
  soilHumidity=map(analogRead(A_SOIL_HUMI),1000,400,0,100);
  if(soilHumidity>100){
    soilHumidity=100;
  }
  else if(soilHumidity<0){
    soilHumidity=0;
  }
}

void writeRGB(bool R, bool G, bool B){
  digitalWrite(RGB_R,R);
  digitalWrite(RGB_G,G);
  digitalWrite(RGB_B,B);
}

void turn_on_pump(){
  delay(2000);
  lcd.clear();
  lcd.noBacklight();
  for(int i=0; i<220; i++){
    analogWrite(PUMP_A,i);
    delay(5);
  }
  delay(On_Time);
  analogWrite(PUMP_A,0);
  analogWrite(PUMP_B,0);
  delay(500);
}

void chkBT(){
  if(bt.available()>0){
    byte data=bt.read();

    if(data==1){
      writeRGB(HIGH,LOW,HIGH);
    }
    else if(data==2){
      writeRGB(LOW,LOW,LOW);
    }
    else if(data==3){
      turn_on_pump();
    }
  }
}

void setup(){
  initPin();
  initLcd();
  delay(2000);
  writeRGB(HIGH,LOW,HIGH);
}

void loop() {
  calcSoilHumidity();
  unsigned long cm=millis();
  if(cm-pm>=1000){
    pm=cm;
    bt.print(soilHumidity);
  }
  printLcd();
  chkBT();
  if(soilHumidity<20){
    bt.print(soilHumidity);
    delay(500);
    turn_on_pump();
  }
}
