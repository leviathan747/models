/**
 * app.h
 * EV3 HRP app header
 */

#include "target_test.h"

#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void balance_task(intptr_t extinf);

extern void timer_start(intptr_t extinf);
extern void cyclic_handler(intptr_t extinf);

#endif
