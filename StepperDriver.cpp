#include "StepperDriver.h"

#include <Arduino.h>
#include <Stepper.h>

const int stepsPerRevolution = 2048 ;
Stepper myStepper(stepsPerRevolution, 8, 7, 6, 5);

void setupStepper() {
  myStepper.setSpeed(8);
}

void rotate(bool reverse, int stepsPercentage) {
  int steps = (stepsPerRevolution * stepsPercentage) / 100;
  if (reverse == true) {
    Serial.println(-steps);
    myStepper.step(-steps);
  } else {
    Serial.println(steps);
    myStepper.step(steps);
  }
}