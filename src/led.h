/*
 * led.h
 *
 *  Created on: Jan 30, 2013
 *      Author: matt
 */

#ifndef LED_H_
#define LED_H_

#include "main.h"

typedef enum{
  IND_N,
  IND_S,
  IND_E,
  IND_W,
  IND_CW,
  IND_CCW,
  IND_INV
} Indication_t;

void led_init();
void led_set_indication(Indication_t ind);
void led_tick();

#endif /* LED_H_ */
