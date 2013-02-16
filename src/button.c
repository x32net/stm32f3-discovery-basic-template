/*
 * button.c
 *
 *  Created on: Feb 10, 2013
 *      Author: matt
 */
#include "stm32f30x_gpio.h"
#include "button.h"

// Pin Definitions:

// PA9 - button Input
// PC9 - button Gnd/Vdd

void button_init(){
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure other pin as Vdd */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIOC->BSRR = GPIO_Pin_9;
}

int button_state(){
  return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
}
