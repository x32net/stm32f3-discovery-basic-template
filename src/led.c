/*
 * led.c
 *
 *  Created on: Jan 30, 2013
 *      Author: matt
 */
#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "led.h"

__IO uint8_t LEDSeq = 0;
__IO Indication_t LEDInd = IND_INV;

void led_all_off(){
  /* LEDs Off */
  STM_EVAL_LEDOff(LED4);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED6);
  STM_EVAL_LEDOff(LED7);
  STM_EVAL_LEDOff(LED10);
  STM_EVAL_LEDOff(LED8);
  STM_EVAL_LEDOff(LED9);
  STM_EVAL_LEDOff(LED5);

  STM_EVAL_LEDOff(ORIG_LED4);
  STM_EVAL_LEDOff(ORIG_LED3);
  STM_EVAL_LEDOff(ORIG_LED6);
  STM_EVAL_LEDOff(ORIG_LED7);
  STM_EVAL_LEDOff(ORIG_LED10);
  STM_EVAL_LEDOff(ORIG_LED8);
  STM_EVAL_LEDOff(ORIG_LED9);
  STM_EVAL_LEDOff(ORIG_LED5);
}

void led_init(){

  /* Initialize LEDs available on STM32F3-Discovery board */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);

  STM_EVAL_LEDInit(ORIG_LED3);
  STM_EVAL_LEDInit(ORIG_LED4);
  STM_EVAL_LEDInit(ORIG_LED5);
  STM_EVAL_LEDInit(ORIG_LED6);
  STM_EVAL_LEDInit(ORIG_LED7);
  STM_EVAL_LEDInit(ORIG_LED8);
  STM_EVAL_LEDInit(ORIG_LED9);
  STM_EVAL_LEDInit(ORIG_LED10);

  led_all_off();
}

void led_set_indication(Indication_t ind){
  LEDInd = ind;
}

void led_do_NS_sweep(){
  led_all_off();
  uint8_t mySeq = LEDSeq/20;
  if(LEDInd == IND_SN){
    mySeq = 4-mySeq;
  }
  switch(mySeq){
  case 0:
    STM_EVAL_LEDOn(LED3);
    break;
  case 1:
    STM_EVAL_LEDOn(LED5);
    STM_EVAL_LEDOn(LED4);
    break;
  case 2:
    STM_EVAL_LEDOn(LED7);
    STM_EVAL_LEDOn(LED6);
    break;
  case 3:
    STM_EVAL_LEDOn(LED9);
    STM_EVAL_LEDOn(LED8);
    break;
  case 4:
    STM_EVAL_LEDOn(LED10);
    break;
  }
}

void led_do_EW_sweep(){
  led_all_off();
  uint8_t mySeq = LEDSeq/20;
  if(LEDInd == IND_WE){
    mySeq = 4-mySeq;
  }
  switch(mySeq){
  case 0:
    STM_EVAL_LEDOn(LED7);
    break;
  case 1:
    STM_EVAL_LEDOn(LED5);
    STM_EVAL_LEDOn(LED9);
    break;
  case 2:
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED10);
    break;
  case 3:
    STM_EVAL_LEDOn(LED8);
    STM_EVAL_LEDOn(LED4);
    break;
  case 4:
    STM_EVAL_LEDOn(LED6);
    break;
  }
}

void led_do_rotation(){
  led_all_off();
  uint8_t mySeq = LEDSeq/25;
  if(LEDInd == IND_CCW){
    mySeq = 3-mySeq;
  }
  switch(mySeq){
  case 0:
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED10);
    break;
  case 1:
    STM_EVAL_LEDOn(LED5);
    STM_EVAL_LEDOn(LED8);
    break;
  case 2:
    STM_EVAL_LEDOn(LED7);
    STM_EVAL_LEDOn(LED6);
    break;
  case 3:
    STM_EVAL_LEDOn(LED9);
    STM_EVAL_LEDOn(LED4);
    break;
  }
}

void led_do_static(){
  switch(LEDInd){
  case IND_IMPOSSIBLE:
    STM_EVAL_LEDOn(LED9);
    STM_EVAL_LEDOn(LED10);
    STM_EVAL_LEDOn(LED8);
  case IND_HARD:
    STM_EVAL_LEDOn(LED7);
    STM_EVAL_LEDOn(LED6);
  case IND_MED:
    STM_EVAL_LEDOn(LED5);
    STM_EVAL_LEDOn(LED4);
  case IND_EASY:
    STM_EVAL_LEDOn(LED3);
    break;
  }
}

void led_tick(){
  if((++LEDSeq) == 100){ //if we need 6, this can be 150
    LEDSeq = 0;
  }
  switch(LEDInd){
  case IND_NS:
  case IND_SN:
    if((LEDSeq % 20) == 0){
      led_do_NS_sweep();
    }
    break;
  case IND_EW:
  case IND_WE:
    if((LEDSeq % 20) == 0){
      led_do_EW_sweep();
    }
    break;
  case IND_CW:
  case IND_CCW:
    if((LEDSeq % 25) == 0){
      led_do_rotation();
    }
    break;
  case IND_EASY:
  case IND_MED:
  case IND_HARD:
  case IND_IMPOSSIBLE:
    led_do_static();
    break;
  default:
    led_all_off();
    break;
  }
}
