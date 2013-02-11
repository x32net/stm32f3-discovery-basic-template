/*
 * servo.c
 *
 *  Created on: Oct 27, 2012
 *      Author: matt
 */

#include "beep.h"

__IO uint8_t BeepOn = 0;

// Pin Definitions:

// PA10 - TIM2_CH4 (AF10)

void beep_init() {
  // Enable the GPIO Clocks
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);

  // configure pins
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_10);

  // Enable Timer clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Time Base configuration */

  /* the internal clock is 72MHz, so if we divide by a /72 prescaler,
   * we get a 1MHZ counter clock so,
   * 400Hz = 2.5ms/cycle
   * 200Hz =   5ms/cycle
   *
   * 2.5ms = 2500 count
   *   5ms = 5000 count
   * "pulse" or "compare" should be half this
   */
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = 71; //72 = 71+1
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 2500;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Channel 1, 2, 3 and 4 Configuration in PWM mode */
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OCInitStructure.TIM_Pulse = 1250;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure); //set up channel 4

  /* TIM1 counter enable */
  TIM_Cmd(TIM2, DISABLE);

  /* TIM1 Main Output Disable */
//  TIM_CtrlPWMOutputs(TIM4, DISABLE); //this doesn't work on TIM4
  BeepOn = 0;
}

void beep_on(uint8_t high){
  beep_init();  //Why do I have to do this?!
  if(high == 1){
    TIM_SetAutoreload(TIM2,1250);
    TIM_SetCompare4(TIM2, 625);
  }
  else{
    TIM_SetAutoreload(TIM2,5000);
    TIM_SetCompare4(TIM2, 2500);
  }
  TIM_Cmd(TIM2, ENABLE);
  BeepOn = 25;
}

void beep_tick(){
  if(BeepOn == 1){
    TIM_Cmd(TIM2, DISABLE);
    BeepOn = 0;
  }
  else if(BeepOn != 0){
    BeepOn--;
  }
}
