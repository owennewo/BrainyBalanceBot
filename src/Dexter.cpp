#include <Arduino.h>
#include <IMU.h>
#include <Motor.h>
#include <PID.h>
#include <Data.h>
#include <Comms.h>

Data data = Data(0.0027, 0.01, 200, 0.5);// = &data;
// Data data = Data(0.0027, 0.01, 0.06, 0.5);// = &data;

void setup() {

  Serial.begin(500000);
  
  delay(3000);

  MotorBegin();
  ImuCalibrate();
  
} 

// smooth out data readings
float smooth(float newValue, float oldValue, float alpha) {
  return alpha * oldValue + (1 - alpha) * newValue;
}

void loop() {

  readSerial();
  
  if (ImuRead()) {
      
      if (ImuCrashed(data.angleMeasured)) {
        MotorDisable();
      } else {
        MotorEnable();
        MotorCalculateMeasuredSpeed();

        data.angleTarget = PidSpeedToAngle(data.speedMeasured, data.speedInput, data.frequency);            
        data.speedTarget = PidAngleToSteps(data.angleMeasured, data.angleTarget, data.frequency);

        data.speedTarget = constrain(data.speedTarget, data.speedMeasured - MAX_ACCEL, data.speedMeasured + MAX_ACCEL);
        data.speedTarget = constrain(data.speedTarget, -MAX_SPEED, MAX_SPEED);
        data.speedTargetLeft = (data.speedTarget - data.directionInput);
        data.speedTargetRight = (data.speedTarget + data.directionInput);
        
        MotorSetLeftSpeed(2 * data.speedTargetLeft);
        MotorSetRightSpeed(2 * data.speedTargetRight);            
      }     
      data.print();
  }
}