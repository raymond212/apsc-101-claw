#include <Servo.h>

#define TRIGGER_PIN 12    
#define ECHO_PIN 11
#define SONAR_GND 10
#define SONAR_VCC 13

#define SERVO_PIN 9

#define LED 7
#define LED_GND 6

#define SWITCH0 A0
#define SWITCH1 A1

#define ROUND_1 3
#define ROUND_2 2

#define MAX_DISTANCE 200  // maximum distance set to 200 cm

#define TRUE 1
#define FALSE 0

Servo myservo;
int pos = 0;

// ROUND 1 VARIABLES
float pickUpHeight = 8.5;
float midHeight = 30; // 30 + 8
float dropHeight = 20;

int openPos = 0;
int closePos = 100;

int closed = FALSE;
int mid = TRUE;

// ROUND 2 VARIABLES
int blockNum = 4;
float blockHeight = 3.25;
float standHeight = blockNum * blockHeight;

float servoHeight = 8.5; // distance from servo to closed claw
float height = standHeight + blockHeight / 2 + servoHeight;

const int r2ClosePos = 83;

// 

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SONAR_GND, OUTPUT);
  pinMode(SONAR_VCC, OUTPUT);

  pinMode(LED, OUTPUT);
  pinMode(LED_GND, OUTPUT);

  // Using pin 10 and 13 to provide power for sonar
  digitalWrite(SONAR_GND, LOW);
  digitalWrite(SONAR_VCC, HIGH);

  // Using pin 2 as LED ground
  digitalWrite(LED_GND, LOW);
  digitalWrite(LED, LOW); // light is initially turned off

  // Using A0 as LOW for round 1/2 switch
  digitalWrite(SWITCH0, LOW);
  // Using A1 as HIGH for round 1/2 switch
  digitalWrite(SWITCH1, HIGH);
  
  myservo.attach(SERVO_PIN);

  moveServo(openPos);

  if (digitalRead(ROUND_2) == HIGH) {
    closePos = r2ClosePos;
    pickUpHeight = height;
    midHeight = pickUpHeight + 5;
    dropHeight = height;
  }

  getDist();
  delay(1000);
  
}

void loop() { 
//  String input = readSerial();
//
//  if (input.length() > 0) {
//    int pos = input.toInt();
//    Serial.println(pos);
//    moveServo(pos);
//  }

  Serial.println("Distance: " + String(getDist()));

  if (mid && !closed && getDist() < pickUpHeight) {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    moveServo(closePos);
    closed = TRUE;
    mid = FALSE;
  } else if (!mid && getDist() > midHeight) {
    mid = TRUE;
  } else if (mid && closed && getDist() < dropHeight) {
    moveServo(openPos);
    closed = FALSE;
    mid = FALSE;
  }    

  delay(500);
}

float getDist() {
  digitalWrite(TRIGGER_PIN, LOW);   // Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);  // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(20);
  digitalWrite(TRIGGER_PIN, LOW);  

  int duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  myservo.write(pos); // reset to 0

  return distance;
}

void moveServo(int target) {
  int increment = 1;
  if (pos == target) {
    return;
  } else if (pos > target) {
    increment = -1;
  }
  while (pos != target) {
    pos += increment;
    myservo.write(pos);
  }
  return;
}

String readSerial() {
  String input;
  while (Serial.available()) {
    delay(3);  //delay to allow buffer to fill 
    if (Serial.available() > 0) {
      char c = Serial.read();  //gets one byte from serial buffer
      input += c; //makes the string readString
    } 
  }
  return input;
}
