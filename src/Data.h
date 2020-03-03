#ifndef _DATA_H_
#define _DATA_H_

struct Coord {
    float x, y, z;
};

struct Axes {
    float roll, pitch, yaw;
};

struct Data {

  char buffer[80];

  bool log = true;

  Coord coordAccel; // units m/s/s i.e. accelZ if often 9.8 (gravity)
  Coord coordGyro; // units dps (degrees per second)
  Coord coordGyroDrift; // units dps
  Axes axesAccel; // units degrees (roll and angle noisy, yaw not possible)
  Axes axesGyro; // units degrees (expect major drift)
  Axes axesGyroCalibrated; // units degrees (expect minor drift)
  Axes axes; // units degrees (excellent roll, angle, yaw minor drift)
  long stepsPerSecond;
  long stepsPerSecondLast;
  long stepsCountLeft;
  long stepsPerSecondLeft;
  long stepsPerSecondLeftDesired;
  long stepsCountRight;
  long stepsPerSecondRight;
  long stepsPerSecondRightDesired;
  long stepsPerSecondOld;
  long stepsPerSecondKp;
  long stepsPerSecondKd;
  long interval;
  float frequency;
  float angleError;
  long speedError;

  long desiredSpeed;
  long desiredDirection;
  float desiredAngle;

    float speedKp; //  = 0.0027;
    float speedKi; // = 0; //0.0006;
    float angleKp; // = 80;
    float angleKd; // = 0.45;

    Data(float _speedKp, float _speedKi, float _angleKp, float _angleKd) {
        speedKp = _speedKp;
        speedKi = _speedKi;
        angleKp = _angleKp;
        angleKd = _angleKd;
    }

    void print() {
        static long count;
        count++;
        if (log) {
            if (count%1==0) {
                Serial.print("# ");
                Serial.print(desiredSpeed); Serial.print(" ");
                Serial.print(desiredDirection); Serial.print(" ");
                Serial.print(speedKp, 4); Serial.print(" ");
                Serial.print(speedKi, 4); Serial.print(" ");
                Serial.print(angleKp, 4); Serial.print(" ");
                Serial.print(angleKd, 4); Serial.print(" ");
                Serial.print(axes.roll, 3); Serial.print(" ");
                Serial.print(axesAccel.roll, 2); Serial.print(" ");
                Serial.print(desiredAngle, 3); Serial.print(" ");
                Serial.print(frequency, 3); Serial.print(" ");
                Serial.print(stepsPerSecond); Serial.print(" ");
                Serial.print(desiredSpeed); Serial.print(" ");
                // Serial.print(stepsPerSecondLeftDesired); Serial.print(" ");
                // Serial.print(stepsPerSecondLeft); Serial.print(" ");
                // Serial.print(stepsPerSecondRightDesired); Serial.print(" ");
                // Serial.print(stepsPerSecondRight); Serial.print(" ");
                // Serial.print(stepsPerSecondKp); Serial.print(" ");
                // Serial.print(stepsPerSecondKd); Serial.print(" ");
                Serial.print(angleError, 4); Serial.print(" ");
                Serial.print(speedError); Serial.print(" ");
                Serial.println("");
            }
        }
        
    }

};




#endif

