#ifndef MOTOR_H

#define MOTOR_H
void MotorEnable();

void MotorDisable();

void MotorInit();

void MotorSetRightSpeed(long stepsPerSecond);
void MotorSetLeftSpeed(long stepsPerSecond);

#endif
