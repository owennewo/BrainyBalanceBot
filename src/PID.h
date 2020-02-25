#ifndef PID_H
#define PID_H

float PidSpeedToPitch(long speed, long setSpeed, float frequency);
long PidPitchToSteps(float angle, float setPitch, float frequency);
void PidStartTwiddling();
float PidTwiddle(long speed, float angle);

#endif