#ifndef TM1638P_H
#define TM1638_H

#define PIN_TM_STB 10
#define PIN_TM_CLK 8
#define PIN_TM_DIO 9

#include <TM1638plus.h>

extern TM1638plus tm;

#define BTN_1 0b000001
#define BTN_2 0b000010
#define BTN_3 0b000100
#define BTN_4 0b001000
#define BTN_5 0b010000
#define BTN_6 0b100000
#define BTN_7 0b01000000
#define BTN_8 0b10000000

#endif