#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "header.h"

#define ULTRA0_TRIG_PIN P1_0
#define ULTRA1_TRIG_PIN P1_1
void ultrasonicInit();
void ultrasonicTrigger();
void initTimer();
#endif

