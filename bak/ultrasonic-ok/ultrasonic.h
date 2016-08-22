#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "header.h"
typedef u16 TimeType;
#define ULTRA_TRIG_PIN P0_0
void ultrasonicInit();
void ultrasonicTrigger();
void initTimer();
#endif

