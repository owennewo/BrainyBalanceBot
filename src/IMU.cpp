#include "Arduino.h"
#include <Arduino_LSM6DS3.h>
#include <IMU.h>

 long _lastTime;
 long _lastInterval = 0;

 extern Data data;

 void ImuBegin()
{
    
    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        while (1);
    }
}

void ImuCalibrate() {

  int i;
  float value = 0;
  float dev;


  int16_t values[100];
  bool gyro_cal_ok = false;
  Serial.print("Calibrating ");
  
  delay(500);
  while (!gyro_cal_ok){
    // Serial.println("Gyro calibration... DONT MOVE!");
    // we take 100 measurements in 4 seconds
    for (i = 0; i < 100; i++)
    {
      if (i%10==0) {
        Serial.print(".");
      }
      if (ImuRead()) {
        values[i] = data.coordGyro.x;
        value += data.coordGyro.x;
        delay(25);
      }
      // MPU6050_read_3axis();
      
    }
    // mean value
    value = value / 100;
    // calculate the standard deviation
    dev = 0;
    for (i = 0; i < 100; i++)
      dev += (values[i] - value) * (values[i] - value);
    dev = sqrt((1 / 100.0) * dev);
    // Serial.print("offset: ");
    // Serial.print(value);
    // Serial.print("  stddev: ");
    // Serial.println(dev);
    if (dev < 5.0)
      gyro_cal_ok = true;
    else {
      Serial.println("\nRepeat, DONT MOVE!");
      Serial.print("Calibrating ");
    }
      
  }
  data.coordGyroDrift.x = value;
  
  // now make sure the initial angle is sensible
  ImuRead();
  data.axes.roll = data.axesAccel.roll;
  data.axes.pitch = data.axesAccel.pitch;
  data.axesGyro.roll = data.axesAccel.roll;
  data.axesGyro.pitch = data.axesAccel.pitch;

//   Serial.println(data.coordGyroDrift.z);
  Serial.println(" complete");
}

void ImuCalculate() {
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

  data.axes.roll = data.axes.roll + (data.coordGyro.x - data.coordGyroDrift.x) / data.frequency;
  data.axes.pitch = data.axes.pitch + (data.coordGyro.y - data.coordGyroDrift.y) / data.frequency;
  data.axes.yaw = data.axes.yaw + (data.coordGyro.z - data.coordGyroDrift.z) / data.frequency;

  data.axes.roll = 0.99 * data.axes.roll + 0.01 * data.axesAccel.roll;
  data.axes.pitch = 0.99 * data.axes.pitch + 0.01 * data.axesAccel.pitch;

  if (count % 100 == 0) {
    // Serial.print(data.axesAccel.roll); Serial.print(" ");
    // Serial.print(data.axes.roll); Serial.print(" ");
    // Serial.print(data.axesAccel.roll); Serial.print(" ");
  }


  // TODO Look at this
  int16_t correction = constrain(data.coordGyro.x, data.coordGyroDrift.x - 10, data.coordGyroDrift.x + 10); // limit corrections...
  data.coordGyroDrift.x = data.coordGyroDrift.x * 0.9995 + correction * 0.0005; // Time constant of this correction is around 20 sec.

}

boolean ImuRead() {  
  
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ) {
    long currentTime = micros();
    _lastInterval = currentTime - _lastTime; // expecting this to be ~104Hz +- 4%
    _lastTime = currentTime;

    data.interval = _lastInterval;
    data.frequency = 1000000 / _lastInterval;

    IMU.readAcceleration(data.coordAccel.x, data.coordAccel.y, data.coordAccel.z);
    IMU.readGyroscope(data.coordGyro.x, data.coordGyro.y, data.coordGyro.z);

    ImuCalculate();

    return true;
  }
  return false;
}

