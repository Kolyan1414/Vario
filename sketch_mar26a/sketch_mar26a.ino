#include <LiquidCrystal.h>                //display lib
#include <math.h>                         //math lib
#include "SparkFunBME280.h"               //bme280 lib
#include "Wire.h"
#include "SPI.h"


BME280 mySensor;                          //global object



float getAltitude(void);
float getspeed(void);
void squeals(float);
void boot_song(void);
void print_lcd(void);



byte simbol_up[8]={
  B00100, B01010, B10101, B00100,         //creating my own symbols
  B00100, B00100, B00100, B00100
};
byte simbol_down[8]={
  B00100, B00100, B00100, B00100,
  B00100, B10101, B01010, B00100
};
byte simbol_degree[8]={
  B11100, B10100, B11100, B0, B0, B0, B0, B0
};
   
float pressure=0.0, zeroPress=0.0, previousPress=0.0;
float temperature=0.0, zeroTemp=0.0;
float _speed=0.0;
float Alitude=0.0;
unsigned long previous_time, time_interval, this_time;
int value=0;



void setup() {                            
  LiquidCrystal lcd(7, 6, 5, 4, 3, 2);    //lcd pin settings
  lcd.begin(16, 2);                       //screen size
  lcd.createChar(1,simbol_up);            //download symbols in lcd
  lcd.createChar(2,simbol_down);
  lcd.createChar(3,simbol_degree);
  lcd.setCursor(0,0);                     //cursor position

  mySensor.settings.commInterface = I2C_MODE; //i2c
  mySensor.settings.I2CAddress = 0x77;        //i2c adress
  mySensor.settings.runMode = 3;              //normal mode
  mySensor.settings.tStandby = 0;             //0.5ms
  mySensor.settings.filter = 2;               //fit param
  
  mySensor.settings.tempOverSample = 1;
  mySensor.settings.pressOverSample = 1;
  mySensor.settings.humidOverSample = 1;
  
  delay(10);                                  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
   
  Serial.begin(9600);                         //for debug
  Serial.setTimeout(2000);
  
  pinMode(11, OUTPUT);                        //squeaker

  while(!mySensor.begin()) delay(10);         //until sens. loads
  
  temperature=zeroTemp=300.0;//readTempC ()+273;              //get temperature in K
  pressure=previousPress=zeroPress=760;//readFloatPressure(); //get pressure

  boot_song();                                                //boot song -\__/-

  previous_time=millis();                                     //get time from ON
}

void loop() {
  //pressure=readFloatPressure();
  //pressure+=readFloatPressure();
  //pressure/=2.0;
  
  value=analogRead(A0);
  
  this_time=millis();
  time_interval=this_time-previous_time;
  previous_time=this_time;

  //temperature=readTempC ()+273;

  pressure=760.0-value/500.0;
  
  Serial.println("spressure=");
  Serial.print(pressure);
  Serial.println('\n');
  
  _speed=getspeed();
  previousPress=pressure;
  
  Serial.println("speed=");
  Serial.print(_speed);
  Serial.println('\n');

  Alitude=getAltitude();
  
  Serial.println("alitude=");
  Serial.print(Alitude);
  Serial.println('\n');
  
  squeals(_speed);
 }

void squeals(float my_speed){
  
  if(my_speed<=-3.0){
    tone(11,342+14*my_speed);
    delay(200);
    return;
  }
  
  if((my_speed>-3.0)&&(my_speed<=-0.6)){
    tone(11,531+77*my_speed);
    delay(200);
    return;
  }
  
  if((my_speed>=0.1)&&(my_speed<5.0)){
    tone(11,531+77*my_speed);
    delay(300-40*my_speed);
    noTone(11);
    delay(300-40*my_speed);
    return;
  } 
   
  if(my_speed>5.0){
    tone(11,531+77*my_speed);
    delay(100);
    noTone(11);
    delay(100);
    return;
  }
   
  if((my_speed>-0.6)&&(my_speed<0.1)){
    noTone(11);
    delay(200);
    return;
  }
}

float getAltitude(void){
  return -29.21537*zeroTemp*log(pressure/zeroPress);
}

float getspeed(void){
  return -29215.37*temperature*(pressure-previousPress)/pressure/((double)time_interval);
}

void boot_song(void){
  tone(11,329);
  delay(500);
  noTone(11);
  delay(200);
  tone(11,500);
  delay(500);
  noTone(11);
  delay(100);
  tone(11,5000);
  delay(100);
  noTone(11);
}

void print_lcd(void){
  
}

