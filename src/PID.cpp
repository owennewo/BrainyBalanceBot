#include <Arduino.h>
#include <Data.h>

#define MAX_SPEED 500

extern Data data;

float speedIntError = 0;

float lastPitchError = 0;

float PidSpeedToPitch(long speed, long setSpeed, float frequency) {
  
  
  static long count;
  count ++;


  float error = setSpeed - speed;
  // int maxPitch = 5;
  // speedIntError += constrain(error / frequency, -10, 10);

  float pitch = -(data.speedKp * error);// + speedKi * speedIntError);
  // pitch = constrain(pitch, -maxPitch, maxPitch);

  // #if LOG_SPEED_PID

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
  //   Serial.print(" p Pitch: ");
  //   Serial.print(- error * data.speedKp);
  //   Serial.print(" i Pitch: ");
  //   Serial.print(- data.speedKi * speedIntError);
  //   Serial.print(" Pitch: ");
  //   Serial.print(pitch);
}
  // #endif
  return pitch;
}

long constrainSteps(long stepsPerSecond) {
  // prevent instabilites at very low rates
  if (stepsPerSecond < 0 && stepsPerSecond > -1) {
    return -1;
  } else if (stepsPerSecond >= 0 && stepsPerSecond < 1) {
    return 1;
  }
  return constrain(stepsPerSecond, -MAX_SPEED, MAX_SPEED);
}

float smoothedDeriveError = 0;

long PidPitchToSteps(float pitch, float setPitch, float frequency) {
  float error = setPitch - pitch;
  // float derivError = (error - lastPitchError) * frequency;
  smoothedDeriveError = (smoothedDeriveError + (error - lastPitchError) * frequency) / 2;
  lastPitchError = error;
  
  data.stepsPerSecondKp = int(data.pitchKp * error);
  data.stepsPerSecondKd = int(data.pitchKd * smoothedDeriveError);
  
  long steps = int(data.pitchKp * error + data.pitchKd * smoothedDeriveError);
  
  steps = constrainSteps(steps);

  // #if LOG_ANGLE_PID
    // Serial.print("Pitch: ");
    // Serial.print(pitch);
    // Serial.print(" SetPitch: ");
    // Serial.print(setPitch);
    // Serial.print(" PropSteps: ");
    // Serial.print(pitchKp * error);
    // Serial.print(" DerivSteps: ");
    // Serial.print(pitchKd * derivError);
    // Serial.print(" Steps: ");
    // Serial.print(steps);
    // Serial.println("");
  // #endif
  return steps;
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
  float PidTwiddle(long speed, float pitch) {
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