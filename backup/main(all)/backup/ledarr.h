#ifndef LEDARR_H
#define LEDARR_H
#include "header.h"
#define LEDARR_DATA_PORT P2
#define LEDARR_RED_PORT P1
#define LEDARR_GRN_PORT P0
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
enum Ledarr_Color {LEDARR_COLOR_NONE,LEDARR_COLOR_RED=1,LEDARR_COLOR_GRN=2,LEDARR_ORG=3};
void ledarr_refresh();
void ledarr_show(uchar *arr);
void ledarr_horInv();
void ledarr_vetInv();
#endif