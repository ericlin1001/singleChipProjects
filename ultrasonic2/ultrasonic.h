#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "header.h"

#define ULTRA0_TRIG_PIN P3_0
#define ULTRA1_TRIG_PIN P3_1
void ultrasonicInit();
void ultrasonicTrigger();
void initTimer();
#endif

