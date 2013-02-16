/*
 * gyro.h
 *
 *  Created on: Feb 11, 2013
 *      Author: matt
 */

#ifndef GYRO_H_
#define GYRO_H_

void GyroInit(void);
void GyroReadAngRate (float* pfData);
void GyroTick();
void GyroZeroAndEnable();
void GyroDisable();
float GyroReadAngPos();

#endif /* GYRO_H_ */
