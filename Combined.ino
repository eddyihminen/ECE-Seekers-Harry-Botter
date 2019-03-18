#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int QTIPinL {47};
int QTIPinM {49};
int QTIPinR {52};
#include <Servo.h>
Servo servoRight;
Servo servoLeft;
long stdDelay = 90;
int thresh = 350;
long longDelay = 100;

void setup() {
  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } 
 pinMode(QTIPinL, OUTPUT);
 servoRight.attach(11);
 servoLeft.attach(12);
 Serial.begin(9600);
}

void loop() {
  uint16_t clear, red, green, blue;

  //tcs.setInterrupt(false);      // turn on LED
  
  tcs.getRawData(&red, &green, &blue, &clear);

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
  if (clear > 300) Serial.println(" MIRROR");
  else{
    Serial.println("");
  }
  delay(1);
  int qti1 = rcTime(QTIPinL);
  int qti2 = rcTime(QTIPinM);
  int qti3 = rcTime(QTIPinR);
  
//  Serial.println("sensor batch");
//  Serial.println(qti1);
//  Serial.println(qti2);
//  Serial.println(qti3);
  
 servoRight.writeMicroseconds(1440);
 servoLeft.writeMicroseconds(1560);

 
 

 // BBW: turn left
 if ((qti1 > thresh) and (qti2 > thresh) and (qti3 < thresh)){ 
 servoRight.writeMicroseconds(1300);
 servoLeft.writeMicroseconds(1350);
 delay(stdDelay);
 servoRight.writeMicroseconds(1440);
 servoLeft.writeMicroseconds(1560);
 delay(1);
 }
 
  // BWW: turn big left
 if ((qti1 > thresh) and (qti2 < thresh) and (qti3 < thresh)){ 
 servoRight.writeMicroseconds(1300);
 servoLeft.writeMicroseconds(1460);
 delay(longDelay);
 servoRight.writeMicroseconds(1440);
 servoLeft.writeMicroseconds(1560);
 delay(1);
 }

 // WBB: turn right
 if ((qti1 < thresh) and (qti2 > thresh) and (qti3 > thresh)){ 
 servoRight.writeMicroseconds(1650);
 servoLeft.writeMicroseconds(1700);
 delay(stdDelay);
 servoRight.writeMicroseconds(1440);
 servoLeft.writeMicroseconds(1560);
 delay(1);
 }

  // WWB: turn big right
 if ((qti1 < thresh) and (qti2 < thresh) and (qti3 > thresh)){ 
 servoRight.writeMicroseconds(1540);
 servoLeft.writeMicroseconds(1700);
 delay(longDelay);
 servoRight.writeMicroseconds(1440);
 servoLeft.writeMicroseconds(1560);
 delay(1);
 }
   // BBB: stop
   if ((qti1 > thresh) and (qti2 > thresh) and (qti3 > thresh)){ 
   servoRight.writeMicroseconds(1500);
   servoLeft.writeMicroseconds(1500);
   delay(1000);
   servoRight.writeMicroseconds(1440);
   servoLeft.writeMicroseconds(1560);
  delay(300);
   Serial.println("BBB");
   }
 


}

long rcTime(int pin) {
 pinMode(pin, OUTPUT);
 digitalWrite(pin, HIGH);
 delayMicroseconds(230);
 pinMode(pin, INPUT);
 digitalWrite(pin, LOW); long
 time = micros(); while
 (digitalRead(pin)); time =
 micros() - time; 
 return time;
} 
