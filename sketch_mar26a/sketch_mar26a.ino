#include <LiquidCrystal.h>                //display lib
#include <TimerOne.h>
#include <math.h>                         //math lib
#include "SparkFunBME280.h"               //bme280 lib
#include "Wire.h"
#include "SPI.h"



BME280 mySensor;                          //global object - sensor
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);      //lcd pin settings & creatnig global object



float getspeed(unsigned long);
void set_temperature(void);
float getAltitude(void);
void boot_song(void);
void print_lcd(void);
void squeals(float);


  
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


   
float zeroPress=0.0, pressure=0.0, previousPress=0.0;
float zeroTemp=0.0, temperature=0.0, tempCelsium=0.0;
float Speed=0.0;
float Alitude=0.0, maxAlitude=0.0;



int value;



unsigned long previous_time, this_time;



void setup() {    
  Timer1.initialize(8388480);                 //set timerone interrupt in 8.4 s
  Timer1.attachInterrupt(set_temperature);    //get new temperatur interrupt
                          
  lcd.begin(16, 2);                           //screen size
  lcd.createChar(0,simbol_up);                //download symbols in lcd
  lcd.createChar(1,simbol_down);
  lcd.createChar(2,simbol_degree);
  lcd.clear();
  lcd.print("Downloading...");

  mySensor.settings.commInterface = I2C_MODE; //i2c
  mySensor.settings.I2CAddress = 0x77;        //i2c adress
  mySensor.settings.runMode = 3;              //normal mode
  mySensor.settings.tStandby = 0;             //0.5ms
  mySensor.settings.filter = 0;               //filter off
  
  mySensor.settings.tempOverSample = 1;       //??
  mySensor.settings.pressOverSample = 1;
  mySensor.settings.humidOverSample = 1;
  
  delay(10);                                  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
   
  pinMode(11, OUTPUT);                        //squeaker

  mySensor.begin();                           //sensor loads

  zeroTemp=temperature=(tempCelsium=mySensor.readTempC())+273.15;             //get temperature in K
  zeroPress=previousPress=pressure=mySensor.readFloatPressure();//get pressure
  
  boot_song();                                                //boot song -\__/-

  previous_time=millis();                                     //get time from ON
}

void loop() {
  pressure=mySensor.readFloatPressure();                      //perhaps, i should fit pressure

  this_time=millis();

  Speed=getspeed(this_time-previous_time);
  previous_time=this_time;
  previousPress=pressure;
  
  Alitude=getAltitude();
  if(Alitude>maxAlitude){
    maxAlitude=Alitude;
  }
  
  print_lcd();
  squeals(Speed);
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
   
  if(my_speed>=5.0){
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

float getAltitude(void){                                  //barametristik?? 
  return -29.21537*zeroTemp*log(pressure/zeroPress);
}

float getspeed(unsigned long time_interval){
  return -29215.37*temperature*(pressure-previousPress)/pressure/((float)time_interval);
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
  return;
}
void print_lcd(void){
  lcd.clear();
  
  if(Speed>=0.0){
    lcd.write(byte(0));
    lcd.print(" +");
    lcd.print(Speed,1);
  }else{
    lcd.write(byte(1));
    lcd.setCursor(2,0);
    lcd.print(Speed,1);
  }

  lcd.print("m/s");

  if((Alitude<10.0)&&(Alitude>-10.0)){
    lcd.setCursor(11,0);
    if(Alitude>=0.0)  lcd.print("+");
    lcd.print(Alitude);
  }else{
    if((Alitude<100.0)&&(Alitude>-100.0)){
      lcd.setCursor(10,0);
      if(Alitude>=0.0)  lcd.print("+");
      lcd.print(Alitude);
    }else{
      if((Alitude<1000.0)&&(Alitude>-1000.0)){
        lcd.setCursor(11,0);
        if(Alitude>=0.0)  lcd.print("+");
        lcd.print(Alitude);
      }else{
        lcd.setCursor(10,0);
        if(Alitude>=0.0)  lcd.print("+");
        lcd.print(Alitude);
      }
    }
  }
  
  lcd.setCursor(15,0);
  lcd.print("m");

  lcd.setCursor(0,1);
  lcd.print("max=");
  lcd.print(maxAlitude,0);
  lcd.print("m");

  if((tempCelsium<10.0)&&(tempCelsium>-10.0)){
    if(tempCelsium>=0.0){
      lcd.setCursor(10,1);
      lcd.print("+");
      lcd.print(tempCelsium);
    }else{
      lcd.setCursor(10,1);
      lcd.print(tempCelsium);
    }
  }else{
    if(tempCelsium>=0.0){
      lcd.setCursor(9,1);
      lcd.print("+");
      lcd.print(tempCelsium);
    }else{
      lcd.setCursor(9,1);
      lcd.print(tempCelsium);
    }
  }

  lcd.setCursor(14,1);
  lcd.print("C");
  lcd.write(byte(2));
  
  return;
}

void set_temperature(void){
  temperature=(tempCelsium=mySensor.readTempC())+273.15;
  return;
}

