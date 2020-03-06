#ifndef _DATA_H_
#define _DATA_H_

#define MAX_SPEED 1200
#define MAX_ANGLE 25
#define MAX_ACCEL 20


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
  
  
  // TIME
  long interval;   // between imu samples ~10ms
  float frequency; // 1 / interval = ~104Hz

  // measured steps  
  long stepsCountLeft;
  long stepsCountRight;
  
  
  // INPUTs from remote
  long speedInput;
  long directionInput;


  // all speeds are measured in steps per second with a (200 steps in a revolutions)
  long speedTarget;
  long speedTargetLeft;
  long speedTargetRight;

  long speedMeasured;
  long speedMeasuredLeft;
  long speedMeasuredRight;

  long speedEstimated;

  long speedError;  

  float angleTarget;
  float angleMeasured;
  float angleError;
  float angularVelocity; 
  
  // PID config
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
        // log = false;
        if (log) {
            if (count%20==0) {
                Serial.print("#");

                // CONFIG
                Serial.print(" kps:");  Serial.print(speedKp, 4);
                Serial.print(" kpi:");  Serial.print(speedKi, 4); 
                Serial.print(" kpa:");  Serial.print(angleKp, 4);
                Serial.print(" kda:");  Serial.print(angleKd, 4);
                
                // DIRECTION
                Serial.print(" di:");   Serial.print(directionInput);

                // ANGLE
                Serial.print(" am:");   Serial.print(angleMeasured, 3); 
                // Serial.print(" ama:");   Serial.print(axesAccel.roll, 2); 
                Serial.print(" at:");    Serial.print(angleTarget, 3); 
                Serial.print(" aerr:");   Serial.print(angleError, 4); 
                Serial.print(" av:");   Serial.print(angularVelocity, 4); 
                
                // TIME
                // Serial.print(" f:");    Serial.print(frequency, 3); 
                
                // SPEED
                Serial.print(" si:");   Serial.print(speedInput); 
                // SPEED TARGET
                // Serial.print(" st:");   Serial.print(speedTarget); 
                // Serial.print(" stl:");   Serial.print(speedTargetLeft); 
                // Serial.print(" str:");  Serial.print(speedTargetRight);
                
                // SPEED MEASURED
                Serial.print(" sm:");    Serial.print(speedMeasured); 
                // Serial.print(" sml:");  Serial.print(speedMeasuredLeft); 
                // Serial.print(" smr:");   Serial.print(speedMeasuredRight);
                
                Serial.print(" sest:");   Serial.print(speedEstimated);
                // SPEED ERROR
                Serial.print(" serr:");   Serial.print(speedError);
                Serial.println("");
            }
        }
        
    }

};




#endif

