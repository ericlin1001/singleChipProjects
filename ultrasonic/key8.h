#ifndef KEY8_H
#define KEY8_H
#include "header.h"

#define Keys P2
#define KEY_COUNT_MAX 10//KEY_COUNT_MAX < 2^7
void processKey();
BOOL isKeyPress(uchar i);
BOOL isKeyDown(uchar i);
BOOL isKeyUp(uchar i);
BOOL isKeyRelease(uchar i);
void clearKeyCount();

#endif