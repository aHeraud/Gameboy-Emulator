#ifndef EMULATOR_H
#define EMULATOR_H

#include<stdbool.h>

extern bool emulator_is_running;

void emulator_init();
void emulator_reset();
void emulator_step();
void emulator_stop();

#endif