#ifndef IMU_H
#define IMU_H
#include <Data.h>

void ImuInit();
void ImuCalibrate();
boolean ImuRead();
float ImuCalculate();
void ImuPrint();


#endif