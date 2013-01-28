/*
 * servo.c
 *
 *  Created on: Oct 27, 2012
 *      Author: matt
 */

#include "servo.h"

// Pin Definitions:

// PA4 - TIM3_CH2 (AF2)

void servo_init() {
  // Enable the GPIO Clocks
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);

  // configure pins
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_2);

  // Enable Timer clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Time Base configuration */
  /*
   * ok, so we want a 1-2ms pulse every 20ms
   * the internal clock is 72MHz, so if we divide by a /72 prescaler,
   * we get a 1MHZ counter clock so,
   *  20ms repeat = 20,000 count
   *   1ms pulse = 1000 count
   * 1.5ms pulse = 1500 count
   *   2ms pulse = 2000 count
   */
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = 71; //72 = 71+1
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 20000;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Channel 1, 2, 3 and 4 Configuration in PWM mode */
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OCInitStructure.TIM_Pulse = 1000;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure); //set up channel 2

  /* TIM1 counter enable */
  TIM_Cmd(TIM3, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void servo_set_pos(uint32_t pos){
  if(pos <= 1000){
    TIM_SetCompare2(TIM3, 1000+pos);
  }
  else{
    //error?
  }
}
