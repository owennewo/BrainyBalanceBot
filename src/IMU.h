#ifndef IMU_H
#define IMU_H
#include <Data.h>

void ImuBegin();
void ImuCalibrate();
boolean ImuRead();
void ImuPrint();
bool ImuCrashed(float angle);

#endif