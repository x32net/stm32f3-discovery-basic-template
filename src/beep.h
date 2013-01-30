/*
 * pwm.h
 *
 *  Created on: Oct 27, 2012
 *      Author: matt
 */

#ifndef BEEP_H_
#define BEEP_H_

#include "stm32f30x_conf.h"

void beep_init();

void beep_on(uint8_t high);
void beep_tick();

#endif /* BEEP_H_ */
