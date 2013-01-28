/*
 * pwm.h
 *
 *  Created on: Oct 27, 2012
 *      Author: matt
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "stm32f30x_conf.h"

void servo_init();

void servo_set_pos(uint32_t pos);

#endif /* SERVO_H_ */
