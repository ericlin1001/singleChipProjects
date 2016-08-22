#ifndef MOTOR_H
#define MOTOR_H
#include "header.h"

#define TURN_MOTOR P0_0
#define MOTOR0 P1_2
#define MOTOR1 P1_3
#define MOTOR_FORWARD {MOTOR0=0;MOTOR1=1;}
#define MOTOR_STOP {MOTOR0=0;MOTOR1=0;}
#define MOTOR_BACKWARD {MOTOR0=1;MOTOR1=0;}
void doAngle(s16 a);
void doMotor(s16 speed,u16 times);

void doAngle(s16 a){
	uchar i=5;
	a+=43;
	while(i--){
		TURN_MOTOR=1;
		mydelay(a);
		TURN_MOTOR=0;
		mydelay(1800-a);
	}
}
void doMotor(s16 speed,u16 times){
	if(speed<0){
		speed=-speed;
		//speed=speed;
		while(times--){
		if(rand()%1000<speed){
		MOTOR_BACKWARD;
		}else{
			MOTOR_STOP;
		}
		}
	}else{
		//speed=speed/2+500;
		while(times--){
		if(rand()%1000<speed){
		MOTOR_FORWARD;
		}else{
			MOTOR_STOP;
		}
		}
	}
}

#endif

