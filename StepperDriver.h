#ifndef BMEREADER_H
#define BMEREADER_H

#include <Arduino.h>
#include <Stepper.h>

void setupStepper();
void rotate(bool reverse, int stepsPercentage);

#endif