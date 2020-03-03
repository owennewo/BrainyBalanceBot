#include <Arduino.h>
#include <Data.h>


#define ITERM_MAX_ERROR 300   // Iterm windup constants for PI control 
#define ITERM_MAX 50000

extern Data data;

float speedIntError = 0;

float lastAngleError = 0;

float PidSpeedToAngle(long speed, long setSpeed, float frequency) {

  // float error;
  // float output;
  static float errorSum;

  data.speedError = setSpeed - speed;
  errorSum += constrain(data.speedError, -ITERM_MAX_ERROR, ITERM_MAX_ERROR);
  errorSum = constrain(errorSum, -ITERM_MAX, ITERM_MAX);

  float angle = data.speedKp * data.speedError + data.speedKi * errorSum / frequency;
  
  static long count;
  count ++;


if (count%50==0) {
  
  // Serial.print("  === SPEED: ");
  //   Serial.print(speed);
  //   // Serial.print(" SET_SPEED: ");
  //   // Serial.print(setSpeed);
  //   Serial.print(" FREQ : ");
  //   Serial.print(frequency);
  //   Serial.print(" : ");
    
  //   // LOG.print("Set Speed: ");
  //   // LOG.print(setSpeed);
  //   // LOG.print(" speed: ");
  //   // LOG.print(speed / 20);
  //   //LOG.print(" Error: ");
  //   //LOG.print(error);
  //   Serial.print(" p Angle: ");
  //   Serial.print(- error * data.speedKp);
  //   Serial.print(" i Angle: ");
  //   Serial.print(- data.speedKi * speedIntError);
  //   Serial.print(" Angle: ");
  //   Serial.print(angle);
}
  // #endif
  return (angle);
}

// long constrainSteps(long stepsPerSecond) {
//   // prevent instabilites at very low rates
//   if (stepsPerSecond < 0 && stepsPerSecond > -1) {
//     return -1;
//   } else if (stepsPerSecond >= 0 && stepsPerSecond < 1) {
//     return 1;
//   }
//   return constrain(stepsPerSecond, -MAX_SPEED, MAX_SPEED);
// }

float smoothedDeriveError = 0;

long PidAngleToSteps(float angle, float setAngle, float frequency) {


static long setAngleOld;
  static long angleOld;
  data.angleError = setAngle - angle;
  
  // Kd is implemented in two parts
  //    The biggest one using only the input (sensor) part not the SetPoint input-input(t-1).
  //    And the second using the setpoint to make it a bit more agressive   setPoint-setPoint(t-1)
  float setAngleDiff = constrain((setAngle - setAngleOld), -8, 8); // We limit the input part...
  float angleDiff = angle - angleOld;
  float steps = data.angleKp * data.angleError + (data.angleKd * (setAngleDiff - angleDiff) * frequency);
  
  angleOld = angle;  // error for Kd is only the input component
  setAngleOld = setAngle;
  return (steps);
  
}

enum TwiddleState {
  NONE,
  FORWARD,
  PAUSING,
  BACKWARD,
  SETTLING
};

TwiddleState twiddleState = NONE;
unsigned long lastTwiddleTime = millis();
long twiddleError = 0;
long previousSpeedCommand = 0;
int acceleration = 1;

void PidStartTwiddling() {
  twiddleState = FORWARD;
  lastTwiddleTime = millis();
  twiddleError = 0;
  Serial.println("Moving forward...");
}

/**
 * Checks twiddle state and returns desired speed
 */
  float PidTwiddle(long speed, float angle) {
  long speedCommand = 0;
  if (twiddleState == NONE) {
    return 0;
  }

  if (twiddleState == FORWARD) {
    speedCommand = 30;
    if (lastTwiddleTime + 3000 < millis()) {
      twiddleState = PAUSING;
      lastTwiddleTime = millis();
      Serial.println("Pausing...");
    }
  }

  if (twiddleState == PAUSING) {
    speedCommand = 0;
    if (lastTwiddleTime + 5000 < millis()) {
      twiddleState = BACKWARD;
      lastTwiddleTime = millis();
      Serial.println("Moving backward...");
    }
  }

  if (twiddleState == BACKWARD) {
    speedCommand = -30;
    if (lastTwiddleTime + 3000 < millis()) {
      twiddleState = SETTLING;
      lastTwiddleTime = millis();
      Serial.println("Settling...");
    }
  }

  if (twiddleState == SETTLING) {
    speedCommand = 0;
    if (lastTwiddleTime + 5000 < millis()) {
      twiddleState = NONE;
      Serial.print("Done! Error = ");
      Serial.println(twiddleError);
    }
  }

  speedCommand = constrain(speedCommand, previousSpeedCommand - acceleration, previousSpeedCommand + acceleration);
  previousSpeedCommand = speedCommand;
  twiddleError += abs(speedCommand - speed);
  return speedCommand;
}