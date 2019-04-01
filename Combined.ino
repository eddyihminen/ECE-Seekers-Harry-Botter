// choose which features to include
bool line_following = true;
bool communication = true;
bool mirror = true;
bool snitch = true;
bool mirrorVerbosity = true; 
bool verbosity = false;
bool lineFollow = false;
bool fifthHashStop = true;

// line_following
#include <Servo.h>
Servo servoRight;
Servo servoLeft;
int QTIPinL {47};
int QTIPinM {49};
int QTIPinR {52};
long stdDelay = 90;
int thresh = 350;
long longDelay = 100;
int turnDelay = 1000;
int totalDelay = 0;
int bbbStop = 1000;
unsigned long time, lastReceive, lastSend, lastMirror, lastSnitch, lastHash = - 10000;
int blinkDelayTime = 2000;
int mirrorDelayTime = 1500;
int snitchDelayTime = 2000;
int hashDelayTime = 1500;
int numMirrors = 0;
int fixedMirrorCount = 0;
int numHashes = 0;
bool turned = false;

// communication
#define UNIQUE_ROBOT_CODE '8'

// Define pins for XBee serial
#define Rx 17 //define transmitting pin
#define Tx 16 //define recieving pin

// Define pins for button and LED inputs/outputs
#define BUTTON_INPUT 10
#define SEND_LED_OUTPUT 5
#define RECEIVE_LED_OUTPUT 7
#define MIRROR_LED_OUTPUT 2
#define SNITCH_LED_OUTPUT 3

// mirror
#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
 
  Serial.begin(9600); // set  ` up keyboard serial
  Serial2.begin(9600); // set up XBee serial
 
  if (line_following) {
    servoRight.attach(11);
    servoLeft.attach(12);
  }
  if (communication) {
    pinMode (BUTTON_INPUT, INPUT); // set up button pin
    pinMode (SEND_LED_OUTPUT, OUTPUT); // set up LED indicating signal sent
    pinMode (RECEIVE_LED_OUTPUT, OUTPUT); // set up LED indicating signal received
  }
  if (mirror) {
    Serial.println("Color View Test!");
  
    if (tcs.begin()) {
      Serial.println("Found sensor");
    }
    pinMode (MIRROR_LED_OUTPUT, OUTPUT); // set up LED indicating signal received 
  }
  
  if (snitch) {
    Serial.println("Color View Test!");
  
    if (tcs.begin()) {
      Serial.println("Found sensor");
    }
    pinMode (SNITCH_LED_OUTPUT, OUTPUT); // set up LED indicating signal received 
  }
}

void loop() {
  totalDelay = 0;
  if (line_following) {
      // line following
    delay(1);
    totalDelay += 1;
    int qti1 = rcTime(QTIPinL);
    int qti2 = rcTime(QTIPinM);
    int qti3 = rcTime(QTIPinR);

    if (verbosity) {
      Serial.print("sensor batch: \t");
      Serial.print(qti1);
      Serial.print("\t");
      Serial.print(qti2);
      Serial.print("\t");
      Serial.println(qti3);
    }
    
   servoRight.writeMicroseconds(1440);
   servoLeft.writeMicroseconds(1560);
  
   
   
  
   // BBW: turn left
   if (! turned){
     if ((qti1 > thresh) and (qti2 > thresh) and (qti3 < thresh)){ 
     servoRight.writeMicroseconds(1300);
     servoLeft.writeMicroseconds(1350);
     delay(stdDelay);
     totalDelay += stdDelay;  
     servoRight.writeMicroseconds(1440);
     servoLeft.writeMicroseconds(1560);
     delay(1);
     totalDelay += 1;
     }
   }
   else {
     if ((qti1 > thresh) and (qti2 > thresh) and (qti3 < thresh)) {
      if (millis() - lastHash > hashDelayTime) {
        numHashes++;
        Serial2.print(numHashes);
        lastHash = millis();
        if (numHashes == fixedMirrorCount){
          servoRight.writeMicroseconds(1500);
          servoLeft.writeMicroseconds(1500);
          searchSnitch();
          delay(30000);
        }
      }
     }
     }
   
    // BWW: turn big left
   if ((qti1 > thresh) and (qti2 < thresh) and (qti3 < thresh)){ 
   servoRight.writeMicroseconds(1300);
   servoLeft.writeMicroseconds(1460);
   delay(longDelay);
   totalDelay += longDelay;
   servoRight.writeMicroseconds(1440);
   servoLeft.writeMicroseconds(1560);
   delay(1);
   totalDelay += 1;
   }
  
   // WBB: turn right
   if ((qti1 < thresh) and (qti2 > thresh) and (qti3 > thresh)){ 
   servoRight.writeMicroseconds(1650);
   servoLeft.writeMicroseconds(1700);
   delay(stdDelay);
   totalDelay += stdDelay;
   servoRight.writeMicroseconds(1440);
   servoLeft.writeMicroseconds(1560);
   delay(1);
   totalDelay += 1;
   }
  
    // WWB: turn big right
   if ((qti1 < thresh) and (qti2 < thresh) and (qti3 > thresh)){ 
   servoRight.writeMicroseconds(1540);
   servoLeft.writeMicroseconds(1700);
   delay(longDelay);
   totalDelay += longDelay;
   servoRight.writeMicroseconds(1440);
   servoLeft.writeMicroseconds(1560);
   delay(1);
   totalDelay += 1;
   }
   // BBB: turn
   if ((qti1 > thresh) and (qti2 > thresh) and (qti3 > thresh)){ 
     servoRight.writeMicroseconds(1300);
     servoLeft.writeMicroseconds(1460);
     delay(turnDelay);
     totalDelay += longDelay;
     servoRight.writeMicroseconds(1440);
     servoLeft.writeMicroseconds(1560);
     delay(1);
     totalDelay += 1;
     numHashes = 1;
     turned = true;
     fixedMirrorCount = numMirrors;
     Serial2.print(numHashes);
     if (numHashes == fixedMirrorCount){
      servoRight.writeMicroseconds(1500);
      servoLeft.writeMicroseconds(1500);
      searchSnitch();
      delay(30000);
     }
     // make sure the bot doesn't run off the table: FOR TESTING
     if (fifthHashStop){
     if (numHashes == 5){
      servoRight.writeMicroseconds(1500);
      servoLeft.writeMicroseconds(1500);
      delay(30000);
     }      
     }
     }
   }
     
  if (communication) {

  // Memory for serial printing string formatting
  char buf[32];

  // If button pressed, send signal
  
  if (millis() - lastSend > blinkDelayTime) {
    digitalWrite(SEND_LED_OUTPUT, LOW);
  }
  if (digitalRead(BUTTON_INPUT)) {
    
    // Send signal
    char outgoing = UNIQUE_ROBOT_CODE;
    Serial2.print(outgoing);

    // Print signal to serial
    sprintf(buf, "Broadcast signal %c", outgoing);
    Serial.println(buf);

    // Blink send indicator LED
    digitalWrite(SEND_LED_OUTPUT, HIGH);
    lastSend = millis(); 
  }

  // If signal received from XBee serial
  
  if (millis() - lastReceive > blinkDelayTime) digitalWrite(RECEIVE_LED_OUTPUT, LOW);
  if (Serial2.available()){

    // Receive signal
    char incoming = Serial2.read();
    
    // Check that signal is not coming from this robot
    if (incoming != UNIQUE_ROBOT_CODE) {

      // Print signal to serial
      sprintf(buf, "Received signal %c", incoming);
      Serial.println(buf);
      
      // Blink receive indicator LED
      digitalWrite(RECEIVE_LED_OUTPUT, HIGH);
      lastReceive = millis();
    }
  }
    // Throttling
    delay(1);  
    totalDelay += 1;
  }
  if (mirror) {
    uint16_t clear, red, green, blue;  
    //tcs.setInterrupt(false);      // turn on LED
    tcs.getRawData(&red, &green, &blue, &clear);

    if (mirrorVerbosity) {
      Serial.print("C:\t"); Serial.print(clear);
      Serial.print("\tR:\t"); Serial.print(red);
      Serial.print("\tG:\t"); Serial.print(green);
      Serial.print("\tB:\t"); Serial.print(blue);
      Serial.println("");
    }
    
      if (millis() - lastMirror > mirrorDelayTime) digitalWrite(MIRROR_LED_OUTPUT, LOW);
      
      if ((clear > 700)and (red > 550)) {
        if (millis() - lastMirror > mirrorDelayTime) {
          Serial.print("MIRROR\t");
          digitalWrite(MIRROR_LED_OUTPUT, HIGH);
          lastMirror = millis();
          numMirrors += 1;
          if (!turned) Serial2.print(numMirrors);
        }
      }
    
  }


  time = millis();
  Serial.print(totalDelay);
  Serial.print("\t");
  Serial.println(time);
}

// line following
long rcTime(int pin) {
pinMode(pin, OUTPUT);
digitalWrite(pin, HIGH);
delayMicroseconds(230);
pinMode(pin, INPUT);
digitalWrite(pin, LOW); long
time = micros(); while
(digitalRead(pin)); time = micros() - time; 
return time;
}

void searchSnitch(){
   float reading = (analogRead(0));
   //Output of Hall Effect Sensor (integer between 0-1024)
  
   float mag = reading*5.0;
  // Converts reading value to volts Signal (Volts) = Anaglog Measurement *([5
  //Volts]/1024)
  
   mag /= 1024.0;
  //Divides by 1024 giving the sensor output signal in volts
  
   mag = mag - 2.50;
  //Subtacts the zero field output of the sensor to provide pole sensing
  
   mag = mag*5000;
  //Sets units of output to Gauss (5mV/Gauss)

   if (millis() - lastSnitch > snitchDelayTime) digitalWrite(SNITCH_LED_OUTPUT, LOW);
   if (abs(mag) > 400) {
     if (millis() - lastSnitch > snitchDelayTime) {
       Serial.print("SNITCH");
       digitalWrite(SNITCH_LED_OUTPUT, HIGH);
       lastSnitch = millis();
     }
   }
   if (verbosity) {
     Serial.print("mag strength = "); // Display sensor value
     Serial.print(mag,4);
     Serial.println(" Gauss");
      delay(1); // 1 second delay
      totalDelay += 1;
    }
  }
