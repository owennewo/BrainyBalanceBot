#ifndef PID_H
#define PID_H

float PidSpeedToAngle(long speed, long setSpeed, float frequency);
long PidAngleToSteps(float angle, float setAngle, float frequency);
void PidStartTwiddling();
float PidTwiddle(long speed, float angle);

#endif