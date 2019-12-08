#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Constants
const int trigPin = 5; // Pin used to trigger sensor
const int echoPin = 4; // Pin used to read data from sensor
const int ledPin = 12;  // LED pin
const int minDistance = 15; // Minimum distance(centimeters)

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *M1 = AFMS.getMotor(1); // Motor 1
Adafruit_DCMotor *M2 = AFMS.getMotor(2); // Motor 2
const int defaultSpeed = 100; // Default motor speed
const int turnSpeed = 100; // Default turn speed
// Used to callibrate individual motor speed
const int M1offset = 0;
const int M2offset = 0;
// time it takes to turn in milliseconds
const int turn90Duration = 500;
const int turn180Duration = 1000;

int distance; // Holds current distance read from the sensor

int getDistance(){ // Check how close the vehicle is to the nearest object
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  return int(duration*0.034/2); // Return distance in cm
}

void forward(int s = defaultSpeed){
  M1->setSpeed(s + M1offset);
  M2->setSpeed(s + M2offset);
  M1->run(FORWARD);
  M2->run(FORWARD);
}

void backward(int s = defaultSpeed){
  M1->setSpeed(s + M1offset);
  M2->setSpeed(s + M2offset);
  M1->run(BACKWARD);
  M2->run(BACKWARD);
}

void turnLeft(int t = turn90Duration){
  M1->setSpeed(turnSpeed + M1offset);
  M2->setSpeed(turnSpeed + M2offset);
  M1->run(FORWARD);
  M2->run(BACKWARD);
  delay(t);
  stopM();
}

void turnRight(int t = turn90Duration){
  M1->setSpeed(turnSpeed + M1offset);
  M2->setSpeed(turnSpeed + M2offset);
  M1->run(BACKWARD);
  M2->run(FORWARD);
  delay(t);
  stopM();
}

void stopM(){ // Stop both motors
  M1->run(RELEASE);
  M2->run(RELEASE);
}

void alert(){
  Serial.println("Too close");
  // Blink LED
  for(int i = 0; i < 2; i++){
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
  }
}

void changeDirection(){
  int leftD, rightD; // Declare variables
  // Turn Left then check distance
  turnLeft();
  leftD = getDistance();
  // Turn 180 degrees to the right then check distance
  turnRight(turn180Duration);
  rightD = getDistance();
  // Check there isn't enough space on both sides, go backward and turn around
  if(leftD <= minDistance && rightD <= minDistance){
    turnRight();
  } // if there isn't enough space on the right side, turn 180
  else if(leftD > rightD){
    turnLeft(turn180Duration);
  }
}

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication

  digitalWrite(ledPin, HIGH); // Turn LED on

  AFMS.begin();
  M1->setSpeed(defaultSpeed + M1offset);
  M2->setSpeed(defaultSpeed + M2offset);
}
void loop() {
  distance = getDistance(); // Check how if there is anything near
  if(distance < minDistance){ // If it's too close
    stopM(); // Stop moving
    alert(); // Blink LED several times
    changeDirection(); // Find new direction and rotate vehicle
  }
  else{
    forward();
  }
}

