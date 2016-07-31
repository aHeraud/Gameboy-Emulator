#ifndef TIMER_H
#define TIMER_H

#include<stdint.h>
#include"mmu.h"

extern uint16_t timer_div;

void timer_reset();
void timer_update();

#endif
