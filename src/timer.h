#ifndef TIMER_H
#define TIMER_H

#include<stdint.h>
#include"mmu.h"

extern uint64_t timer_counter;

void timer_reset();
void timer_update();

#endif
