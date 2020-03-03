#ifndef MOTOR_H

#define MOTOR_H

void MotorBegin();

void MotorEnable();
void MotorDisable();


void MotorSetRightSpeed(long stepsPerSecond);
void MotorSetLeftSpeed(long stepsPerSecond);
void MotorCalculateMeasuredSpeed();

#endif
