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

  led_all_off();
}

void led_set_indication(Indication_t ind){
  LEDInd = ind;
  LEDSeq = 0;
}

void led_do_NS_sweep(){
  led_all_off();
  uint8_t mySeq = LEDSeq/25;
  if(LEDInd == IND_SN){
    mySeq = 3-mySeq;
  }
  switch(mySeq){
  case 0:
    if(LEDInd == IND_NS){
      STM_EVAL_LEDOn(LED3);
    }
    else{
      STM_EVAL_LEDOn(LED10);
    }
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
  }
}

void led_do_EW_sweep(){
  led_all_off();
  uint8_t mySeq = LEDSeq/25;
  if(LEDInd == IND_WE){
    mySeq = 3-mySeq;
  }
  switch(mySeq){
  case 0:
    if(LEDInd == IND_EW){
      STM_EVAL_LEDOn(LED7);
    }
    else{
      STM_EVAL_LEDOn(LED6);
    }
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

void led_tick(){
  if((++LEDSeq) == 100){ //if we need 6, this can be 150
    LEDSeq = 0;
  }
  if((LEDSeq % 25) == 0){
    switch(LEDInd){
    case IND_NS:
    case IND_SN:
      led_do_NS_sweep();
      break;
    case IND_EW:
    case IND_WE:
      led_do_EW_sweep();
      break;
    case IND_CW:
    case IND_CCW:
      led_do_rotation();
      break;
    default:
      led_all_off();
      break;
    }
  }
}
