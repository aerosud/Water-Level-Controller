// Green Energy Projects VBL Water level Controller.
// LCD=RS-8,EN-9,DB4-4,DB5-5,DB6-6,DB7-7,OR I2C=SDA,SCL,
// Sd card=MISO-12,MOSI-11,SCK-13,CS-10,
// DS1307RTC=SDA-A4,SCL-A5,

#include <LiquidCrystal_I2C.h>
#include<SD.h>
#include<SPI.h>
#include<Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;

#define trigger1  A0 //Tank
#define    echo1  A1 //Tank
#define    pump1  2    //Water pump1
#define    pump2  3    //Water Pump2
#define     alm1  4     //Tank  Low Alaram
#define     alm2  5     //Tank High Alaram
#define     trip  9     //Pump1 OR Pump2 Trip Alaram
#define pump1_feedback 6
#define pump2_feedback 7
#define        backlit 8   // LCD Backlight
#define    chipSelect 10

int i = 0;

const char* filename = "tankdata.txt";
LiquidCrystal_I2C lcd(0x27,16,2);
 //int time1=0, distance=0, tankLevel=0;
float time1=0, distance=0, tankLevel=0;
File tank_data;

void setup()
{
 Serial.begin(9600);
 rtc.begin(); 
 if(!rtc.isrunning()){
 Serial.println("RTC is not running");
 //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 }
 //rtc.adjust(DateTime(2020,12,18,14,30,0));
 
 lcd.init();
 lcd.backlight();
 tank_data = SD.open(filename, FILE_WRITE);
  
 pinMode(trigger1,OUTPUT);
 pinMode(echo1,INPUT);
 pinMode(pump1,OUTPUT);
 pinMode(pump2,OUTPUT);
 pinMode(alm1,OUTPUT);
 pinMode(alm2,OUTPUT); 
 pinMode(trip,OUTPUT);
 pinMode(pump1_feedback,INPUT);
 pinMode(pump2_feedback,INPUT);
 pinMode(backlit,INPUT);
 pinMode(chipSelect,OUTPUT);
 
 lcd.print(" Green Energy ");
 lcd.setCursor(2,1);
 lcd.print(" Projects ");
 delay(5000);
 lcd.clear();
 
 lcd.print(" Over Head Tank ");
 lcd.setCursor(0,1);
 lcd.print("WaterLevelContrl") ;
 delay(3000);
 Serial.println("GREEN ENERGY PROJECTS"); 
 delay(500);
 Serial.println("Over Head Tank Water Level Controller"); 
 delay(500);
 if(SD.begin(chipSelect)){
   Serial.println("SD card has been initialiazed");
   delay(500);
 }
 
 tank_data = SD.open(filename, FILE_WRITE);
 tank_data.flush();
 tank_data.println(" ");
 tank_data.println(" Power ON");
 tank_data.println(" GREEN ENERGY PROJECTS");
 tank_data.println(" Over Head Tank Water Level Controller");
 }

void loop()
{ 
 int sd_write = 1;
 DateTime now = rtc.now();
 Serial.print(" ");
 Serial.print(now.day(),DEC);
 Serial.print("/");
 Serial.print(now.month(), DEC);
 Serial.print("/");
 Serial.print(now.year(), DEC);
 Serial.print("  "); 
 Serial.print(now.hour(), DEC);
 Serial.print(':');
 Serial.print(now.minute(), DEC);
 Serial.print(':');
 Serial.println(now.second(), DEC);

    tank_data.println();
    tank_data.print("  ");
    tank_data.print(now.day(),DEC);
    tank_data.print("/");
    tank_data.print(now.month(), DEC);
    tank_data.print("/");
    tank_data.print(now.year(), DEC);         
    tank_data.print("   ");
    tank_data.print( now.hour(), DEC);
    tank_data.print(':');
    tank_data.print(now.minute(), DEC);
    tank_data.print(':');
    tank_data.print(now.second(), DEC);
    tank_data.println("");    

 digitalWrite(trigger1,LOW);
 delayMicroseconds(2);
 digitalWrite(trigger1,HIGH);
 delayMicroseconds(10);
 digitalWrite(trigger1,LOW);
 delayMicroseconds(2);
 time1=pulseIn(echo1,HIGH); 
 distance=time1*340/20000;
 if(distance>450){distance = 450;}
 tankLevel=100-(distance*100/450);
 if(distance<=0){
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Tank Sensor Fail");
   Serial.println("\t\tTank Sensor Fail");
   tank_data.println("\t\tTank Sensor Fail");
   digitalWrite(pump1,HIGH);
   digitalWrite(alm1,HIGH);
   delay(1000);
   digitalWrite(alm1,LOW);
 }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TankLevel:");
  lcd.print(int(tankLevel));
  lcd.print("%");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PUMP1 :");
  if(digitalRead(pump1_feedback)){
    lcd.print("ON"); 
    Serial.println("\t\t\tPUMP1 ON ");
    tank_data.println("\t\t\tPUMP1 ON ");     
  }
  else{
    lcd.print("OFF");
    Serial.println("\t\t\tPUMP1 OFF");
    tank_data.println("\t\t\tPUMP1 OFF");
    }
  lcd.setCursor(0,1);
  lcd.print("PUMP2 :");
  if(digitalRead(pump2_feedback)){
    lcd.print("ON");
    Serial.println("\t\t\tPUMP2 ON");
    tank_data.println("\t\t\tPUMP2 ON");  
  }
  else{
    lcd.print("OFF");
    Serial.println("\t\t\tPUMP2 OFF");
    tank_data.println("\t\t\tPUMP2 OFF");
    }
    
   if(tankLevel<75 || (distance<=0)){
    digitalWrite(pump1,HIGH);
    delay (1000);
   if(!digitalRead(pump1_feedback)){
    lcd.setCursor(12,0);
    lcd.print("TRIP");
    Serial.println("\t\t\tPUMP1 TRIP");
    tank_data.println("\t\t\tPUMP1 TRIP");
    digitalWrite(trip,HIGH);
    delay(2000);
    digitalWrite(trip,LOW);
    }
  if(!digitalRead(pump1_feedback)){
    digitalWrite(pump2,HIGH);}
   }
    delay(1000);

     if(tankLevel>80 && !(distance<=0)){
   digitalWrite(pump1,LOW);
   digitalWrite(pump2,LOW);
 }
 
 if(tankLevel<65 && tankLevel>50 ){
  digitalWrite(pump2,HIGH);
  delay(1000);
  if(!digitalRead(pump2_feedback)){
    lcd.setCursor(12,1);
    lcd.print("TRIP");
    Serial.println("\t\t\tPUMP2 TRIP");
    tank_data.println("\t\t\tPUMP2 TRIP"); 
    digitalWrite(trip,HIGH);
    delay(2000);
    digitalWrite(trip,LOW);
   } }
   delay(1000);
 if( tankLevel>75 && digitalRead(pump1_feedback)){
   digitalWrite(pump2,LOW);}
 
 if(digitalRead(backlit)==HIGH){
   lcd.backlight(); }
 if(digitalRead(backlit)==LOW){
   lcd.noBacklight();  }
   
  if(tankLevel<40){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tank Level Low");
    Serial.println("\t\tTANK LEVEL LOW");
    tank_data.println("\t\tTANK LEVEL LOW");
    delay(500);    
    digitalWrite(alm1,HIGH);
    delay(1000);
    digitalWrite(alm1,LOW);}    
 // else{digitalWrite(alm1,LOW);} 
   
  if(tankLevel>75) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tank Level High");
    Serial.println("\t\tTANK LEVEL HIGH");
    tank_data.println("\t\tTANK LEVEL HIGH");
    delay(500); 
    digitalWrite(alm2,HIGH);
    delay(1000);
    digitalWrite(alm2,LOW);
    }
 // else{ digitalWrite(alm2,LOW); } 
 delay(500);
 Serial.print("\t\tTankLevel:");
 Serial.print(tankLevel);
 Serial.print("%\t ");
 Serial.print("Distance:");
 Serial.print(distance/1);
 Serial.print("Cm\n");

  tank_data.print("\t\tTankLevel:");
  tank_data.print(tankLevel);
  tank_data.print("%");
  tank_data.print('\t'); 
  tank_data.print("Distance:");
  tank_data.print(distance/1);
  tank_data.print("Cm");
  tank_data.println();
  tank_data.flush();
  delay(500); 
 i++;
 }
 
