#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "header.h"
typedef ulong TimeType;
#define ULTRA_TRIG_PIN P1_0
void ultrasonicInit();
void ultrasonicTrigger();
void initTimer();
#endif

