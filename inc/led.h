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
  IND_NS,
  IND_SN,
  IND_EW,
  IND_WE,
  IND_CW,
  IND_CCW,
  IND_EASY,
  IND_MED,
  IND_HARD,
  IND_IMPOSSIBLE,
  IND_INV
} Indication_t;

void led_init();
void led_all_off();
void led_set_indication(Indication_t ind);
void led_tick();

#endif /* LED_H_ */
