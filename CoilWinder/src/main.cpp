#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

#define STEPS_PER_MM 533.33
#define STEPS_PER_REV 666.67
#define MAX_SPEED 750

#define COIL_PITCH 1.68
#define NUM_TURNS 18
#define MAX_NUM_TURNS 18
#define NUM_LAYERS 8

// defines pins numbers
const int enable_pin = 11;
const int ms1_pin = 10;
const int ms2_pin = 9;
const int ms3_pin = 8;
const int step1_pin = 7; 
const int dir1_pin = 6; 
const int step2_pin = 5; 
const int dir2_pin = 4;

AccelStepper stepper1 = AccelStepper(1, step1_pin, dir1_pin);
AccelStepper stepper2 = AccelStepper(1, step2_pin, dir2_pin);
MultiStepper steppers;

void setup() {
  Serial.begin(115200);

  pinMode(enable_pin,OUTPUT); 
  pinMode(ms1_pin,OUTPUT);
  pinMode(ms2_pin,OUTPUT);
  pinMode(ms3_pin,OUTPUT);

  digitalWrite(enable_pin, LOW);
  digitalWrite(ms1_pin, LOW);
  digitalWrite(ms2_pin, LOW);
  digitalWrite(ms3_pin, LOW);

  stepper1.setMaxSpeed(MAX_SPEED);
  stepper2.setMaxSpeed(MAX_SPEED);

  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
}

void move_relative(float distance, float turns){
  long positions[2] = {stepper1.currentPosition() + distance * STEPS_PER_MM, 
                       stepper2.currentPosition() + turns * STEPS_PER_REV};

  steppers.moveTo(positions);
  steppers.runSpeedToPosition();

  Serial.print("X = "); Serial.print((float)positions[0]/STEPS_PER_MM);
  Serial.print(", T = "); Serial.println((float)positions[1]/STEPS_PER_REV);
}

void loop() {
   if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    
    if (incomingByte == '7') move_relative(0.25, 0);
    if (incomingByte == '4') move_relative(1, 0);
    if (incomingByte == '1') move_relative(5, 0);

    if (incomingByte == '9') move_relative(-0.25, 0);
    if (incomingByte == '6') move_relative(-1, 0);
    if (incomingByte == '3') move_relative(-5, 0);

    if (incomingByte == '8') move_relative(-0.1*COIL_PITCH, -0.1);
    if (incomingByte == '2') move_relative(0.1*COIL_PITCH, -0.1);
    if (incomingByte == '-') move_relative(0, 0.1);

    if (incomingByte == '5') {
      const float OVERDRIVE_INIT = 2.5;
      const float OVERDRIVE = 1.25;
      const float LENGTH = 30.5;

      int dir = 1;
      for (int i = 0; i < NUM_LAYERS; i++){
        move_relative(-dir * ((i==0)? OVERDRIVE_INIT : OVERDRIVE), 0);
        move_relative(dir * LENGTH, -min(NUM_TURNS + i, MAX_SPEED));
        move_relative(dir * ((i==0)? OVERDRIVE_INIT : OVERDRIVE), 0);
        dir *= -1;
      }
    }
   }
}
