#ifndef HEADER_H
#define HEADER_H
//
#include <reg51.h>
#include <intrins.h>
typedef unsigned char uchar;
typedef unsigned char u8;
typedef unsigned int u16;
 //
#define MAIN_CHOOSE(mainFunc) \
int mainFunc();\
int main(){\
return mainFunc();\
}
#endif