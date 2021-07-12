#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

#define STEPS_PER_MM 533.33
// #define STEPS_PER_REV 666.67
#define STEPS_PER_REV 667.337
#define MAX_SPEED 750

#define COIL_PITCH 1.68
#define NUM_TURNS 18
#define MAX_NUM_TURNS 18
#define NUM_LAYERS 8

// defines pins numbers
// enable, msX_pin are configuration pins on the A49882 stepper motor driver
const int enable_pin = 11;
const int ms1_pin = 10;
const int ms2_pin = 9;
const int ms3_pin = 8;
const int step2_pin = 7; 
const int dir2_pin = 6;

AccelStepper stepper2 = AccelStepper(1, step2_pin, dir2_pin);
MultiStepper steppers;

long turns_total = 0;
long turns_completed = 0;

boolean asked = false;

//https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

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


  stepper2.setMaxSpeed(MAX_SPEED);
  steppers.addStepper(stepper2);
  stepper2.setCurrentPosition(0);
}

void make_turns(float turns){

    long position_target = stepper2.currentPosition() + turns * STEPS_PER_REV;
    
    long positions[1] = {position_target};
    steppers.moveTo(positions);
    steppers.runSpeedToPosition();
}


void loop() { 

    String incomingCommand;      
    if (Serial.available() > 0) {
        incomingCommand = Serial.readStringUntil('\n');
        Serial.println(incomingCommand);
    } else {
        incomingCommand = "continue";
    }

    if (incomingCommand.toInt()) {
        asked = false;
        int turns = incomingCommand.toInt();
        Serial.print("Initializing ");
        Serial.print(turns);
        Serial.println(" turns");

        turns_total = turns;
        turns_completed = 0;
    } else {
        if (abs(turns_total-turns_completed) > 0) {
            make_turns(sgn(turns_total)*1);
            turns_completed += sgn(turns_total) * 1;
            Serial.print("Performed turn #");
            Serial.print(turns_completed);
            Serial.print(" of ");
            Serial.print(turns_total);
            Serial.println(" turns");
        } else {
            if (!asked) {
                asked = true;
                Serial.println("Please type a number or stop");
            }
        }
    }
}
