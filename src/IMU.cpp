#include "Arduino.h"
#include <Arduino_LSM6DS3.h>
#include <IMU.h>
// #include <imu.h>

 long _lastTime;
 long _lastInterval = 0;

 extern Data data;

 void ImuInit()
{
    // data = data;
}

void ImuCalibrate() {
    unsigned int delayMillis = 500;
    unsigned int calibrationMillis = 500;

    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        while (1);
    }

    // Serial.println("imu calibrate");
    int calibrationCount = 0;

  delay(delayMillis); // to avoid shakes after pressing reset button

  float sumX = 0.0, 
        sumY = 0.0, 
        sumZ = 0.0;

  uint startTime = millis();
  while (millis() < startTime + calibrationMillis) {
    // Serial.print(".");
    if (ImuRead()) {
      // in an ideal world coordGyro.x/Y/Z == 0, anything higher or lower represents drift
      sumX += data.coordGyro.x;
      sumY += data.coordGyro.y;
      sumZ += data.coordGyro.z;

      calibrationCount++;
    }
    
  }
  Serial.println("imu done");
  Serial.print("gyro ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.print(" acc ");
  Serial.println(IMU.accelerationSampleRate());
  if (calibrationCount == 0) {
    Serial.println("Failed to calibrate");
  }

  data.coordGyroDrift.x = sumX / calibrationCount;
  data.coordGyroDrift.y = sumY / calibrationCount;
  data.coordGyroDrift.z = sumZ / calibrationCount;

  
  // now make sure the initial angle is sensible
  ImuRead();
  ImuCalculate();
  data.axes.roll = data.axesAccel.roll;
  data.axes.pitch = data.axesAccel.pitch;
  data.axesGyro.roll = data.axesAccel.roll;
  data.axesGyro.pitch = data.axesAccel.pitch;

//   Serial.println(data.coordGyroDrift.z);

  
    
}

boolean ImuRead() {  
  
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ) {
    long currentTime = micros();
    // USB.println(currentTime);
    // USB.println(_lastTime);
    // USB.println(_lastInterval);
    _lastInterval = currentTime - _lastTime; // expecting this to be ~104Hz +- 4%
    _lastTime = currentTime;

    data.interval = _lastInterval;
    data.frequency = 1000000 / _lastInterval;

    IMU.readAcceleration(data.coordAccel.x, data.coordAccel.y, data.coordAccel.z);
    IMU.readGyroscope(data.coordGyro.x, data.coordGyro.y, data.coordGyro.z);
    // doCalculations();
    // Serial.println(data.coordGyro.z);
    //  Serial.println(data.axes.roll);
    return true;
  }
  return false;
}

float ImuCalculate() {
    static int count;
    count++;
  data.axesAccel.roll = atan2(data.coordAccel.y, data.coordAccel.z) * 180 / M_PI;
  data.axesAccel.pitch = atan2(-data.coordAccel.x, sqrt(data.coordAccel.y * data.coordAccel.y + data.coordAccel.z * data.coordAccel.z)) * 180 / M_PI;

  data.axesGyro.roll = data.axesGyro.roll + (data.coordGyro.x / data.frequency);
  data.axesGyro.pitch = data.axesGyro.pitch + (data.coordGyro.y / data.frequency);
  data.axesGyro.yaw = data.axesGyro.yaw + (data.coordGyro.z / data.frequency);

  data.axesGyroCalibrated.roll = data.axesGyroCalibrated.roll + ((data.coordGyro.x - data.coordGyroDrift.x) / data.frequency);
  data.axesGyroCalibrated.pitch = data.axesGyroCalibrated.pitch + ((data.coordGyro.y - data.coordGyroDrift.y) / data.frequency);
  data.axesGyroCalibrated.yaw = data.axesGyroCalibrated.yaw + ((data.coordGyro.z - data.coordGyroDrift.z) / data.frequency);

  data.axes.roll = data.axes.roll + ((data.coordGyro.x - data.coordGyroDrift.x) / data.frequency);
  data.axes.pitch = data.axes.pitch + ((data.coordGyro.y - data.coordGyroDrift.y) / data.frequency);
  data.axes.yaw = data.axes.yaw + ((data.coordGyro.z - data.coordGyroDrift.z) / data.frequency);

  data.axes.roll = 0.98 * data.axes.roll + 0.02 * data.axesAccel.roll;
  data.axes.pitch = 0.98 * data.axes.pitch + 0.02 * data.axesAccel.pitch;
  if (count % 100 == 0) {
    //   Serial.println(count);
        // ImuPrint();
  }
  
  return data.axes.pitch;
}

void ImuPrint() {
// //   Serial.print(data.axesGyro.roll);
// //   Serial.print(',======');
//   Serial.print(data.axesGyro.pitch);
// //   Serial.print(',');
// //   Serial.print(data.axesGyro.yaw);
// //   Serial.print(',');
//   Serial.print(data.axesGyroCalibrated.roll);
//   Serial.print(',');
//   Serial.print(data.axesGyroCalibrated.pitch);
// //   Serial.print(',');
// //   Serial.print(data.axesGyroCalibrated.yaw);
// //   Serial.print(',');
// //   Serial.print(data.axesAccel.roll);
//   Serial.print(',===');
  Serial.print(data.axesAccel.pitch);
//   Serial.print(',');
//   Serial.print(data.axesAccel.yaw);
//   Serial.print(',');
//   Serial.print(data.axes.roll);
  Serial.print("  **   ");
  Serial.print(data.axes.pitch);
//   Serial.print(',');
//   Serial.print(data.axesAccel.yaw);
//   Serial.print(',');
//   Serial.print(data.interval);
//   Serial.print(',');
//   Serial.print(data.frequency);
  Serial.println("");
}