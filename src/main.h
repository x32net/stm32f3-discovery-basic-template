/*
 * main.h
 *
 *  Created on: Jan 30, 2013
 *      Author: matt
 */

#ifndef MAIN_H_
#define MAIN_H_

/* Private types -------------------------------------------------------------*/
typedef enum{
  NORTH = 0,
  SOUTH = 1,
  EAST = 2,
  WEST = 3,
  UP = 4,
  DOWN = 5
} Direction_t;

typedef enum{
  STATE_INIT,
  STATE_OPEN,
  STATE_GYRO,
  STATE_PUZZLE,
  STATE_WON
} State_t;

#endif /* MAIN_H_ */
