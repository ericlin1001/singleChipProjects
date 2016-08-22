#include "header.h"
#include "key8.h"
#include "ledarr.h"
static ulong mainLoopCount=0;

/***************ledarr**************/
/**
LEDARR_DATA_PORT controls row.
LEDARR_RED_PORT/LEDARR_GRN_PORT controls cols;
Low----High
 |
 |
 |
High
LEDARR_DATA_PORT is high active.
LEDARR_RED_PORT/LEDARR_GRN_PORT  is low active.
***/
static uchar ledarr_outputBuffer[8]={0x0,0x0,0x20,0x20,0x7e,0x20,0x20,0};
static uchar ledarr_colSelect[8]={~(1<<0),~(1<<1),~(1<<2),~(1<<3),~(1<<4),~(1<<5),~(1<<6),~(1<<7)};
static uchar ledarr_color=LEDARR_COLOR_RED;
static uchar ledarr_col=0;
BOOL isVetInv=FALSE;
void ledarr_refresh(){
	ledarr_col++;ledarr_col%=8;
	LEDARR_DATA_PORT=ledarr_outputBuffer[ledarr_col];
	if(ledarr_color&0x01){
		LEDARR_RED_PORT=ledarr_colSelect[ledarr_col];
	}else{
		LEDARR_RED_PORT=0xff;//close
	}
	if(ledarr_color&0x02){
		LEDARR_GRN_PORT=ledarr_colSelect[ledarr_col];
	}else{
		LEDARR_GRN_PORT=0xff;//close
	}
}
void ledarr_show(uchar *arr){
	uchar i;
	for(i=0;i<8;i++){
		if(isVetInv)ledarr_outputBuffer[i]=bit_inv(arr[i]);
		else ledarr_outputBuffer[i]=arr[i];
	}
}
void ledarr_horInv(){
uchar i;
	for(i=0;i<8;i++){
		ledarr_colSelect[i]=bit_inv(ledarr_colSelect[i]);
	}

}
void ledarr_vetInv(){
uchar i;
	for(i=0;i<8;i++){
		ledarr_outputBuffer[i]=bit_inv(ledarr_outputBuffer[i]);
	}
	isVetInv=!isVetInv;
}
/***************end ledarr**************/

#define _DO_TEST
/**************test some thing****/
#ifdef main
#undef main
#endif
#define main test_main
#ifdef main

#endif
#ifdef main
#undef main
#endif
/************end test****/

unsigned char code digittab[][8]={
{0x00,0x00,0x3e,0x41,0x41,0x41,0x3e,0x00}, //0
{0x00,0x00,0x00,0x00,0x21,0x7f,0x01,0x00}, //1
{0x00,0x00,0x27,0x45,0x45,0x45,0x39,0x00}, //2
{0x00,0x00,0x22,0x49,0x49,0x49,0x36,0x00}, //3
{0x00,0x00,0x0c,0x14,0x24,0x7f,0x04,0x00}, //4
{0x00,0x00,0x72,0x51,0x51,0x51,0x4e,0x00}, //5
{0x00,0x00,0x3e,0x49,0x49,0x49,0x26,0x00}, //6
{0x00,0x00,0x40,0x40,0x40,0x4f,0x70,0x00}, //7
{0x00,0x00,0x36,0x49,0x49,0x49,0x36,0x00}, //8
{0x00,0x00,0x32,0x49,0x49,0x49,0x3e,0x00}, //9
{0x00,0x00,0x7F,0x48,0x48,0x30,0x00,0x00}, //P
{0x00,0x00,0x7F,0x48,0x4C,0x73,0x00,0x00}, //R
{0x00,0x00,0x7F,0x49,0x49,0x49,0x00,0x00}, //E
{0x00,0x00,0x3E,0x41,0x41,0x62,0x00,0x00}, //C
{0x00,0x00,0x7F,0x08,0x08,0x7F,0x00,0x00}, //H
{0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00}, //I
{0x00,0x7F,0x10,0x08,0x04,0x7F,0x00,0x00}, //N
{0x7C,0x48,0x48,0xFF,0x48,0x48,0x7C,0x00}  //中
};
static void init(){
ledarr_show(digittab[0]);
ledarr_vetInv();
}

static void mainLoop(){
static uchar pWord=0;
#define SELECT 1

#if SELECT==1
if(mainLoopCount%500==0){
ledarr_show(digittab[++pWord]);
pWord%=12;
}

if(isKeyPress(0)){
ledarr_show(digittab[++pWord]);
}
if(isKeyPress(1)){
ledarr_show(digittab[--pWord]);
}
if(isKeyPress(2)){
ledarr_horInv();
}
if(isKeyPress(3)){
ledarr_vetInv();
}
if(isKeyPress(4)){
ledarr_color=LEDARR_COLOR_RED;
}
if(isKeyPress(5)){
ledarr_color=LEDARR_COLOR_GRN;
}
if(isKeyPress(6)){
ledarr_color=LEDARR_ORG;
}
if(isKeyPress(7)){
ledarr_color=LEDARR_COLOR_NONE;
}
#endif

#if SELECT==2
	
#endif
#if SELECT==0
	
#endif



}


int ledarr_main(){
#ifdef DO_TEST
test_main();
#endif
	mainLoopCount=0;
	init();
	while(1)
	{
		processKey();
		ledarr_refresh();
		mainLoop();
		mainLoopCount++;
	}
	return 0;
}
