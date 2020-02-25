#ifndef _DATA_H_
#define _DATA_H_


// void printField(String value, int len = 7) {
//     Serial.print("| ");
//     for(int i = value.length(); i < len; i++)
//         value += ' ';
//     Serial.print(value);
// }

// void printValue(float value, int len = 7) {
//     String strValue = String(value);
//     printField(strValue, len);
// }

struct Coord {
    float x, y, z;
};

struct Axes {
    float roll, pitch, yaw;
};

struct Data {

  char buffer[80];

  Coord coordAccel; // units m/s/s i.e. accelZ if often 9.8 (gravity)
  Coord coordGyro; // units dps (degrees per second)
  Coord coordGyroDrift; // units dps
  Axes axesAccel; // units degrees (roll and pitch noisy, yaw not possible)
  Axes axesGyro; // units degrees (expect major drift)
  Axes axesGyroCalibrated; // units degrees (expect minor drift)
  Axes axes; // units degrees (excellent roll, pitch, yaw minor drift)
  long stepsPerSecond;
  long stepsPerSecondKp;
  long stepsPerSecondKd;
  long interval;
  float frequency;

  long desiredSpeed;
  long desiredDirection;
  float desiredPitch;

    float speedKp; //  = 0.0027;
    float speedKi; // = 0; //0.0006;
    float pitchKp; // = 80;
    float pitchKd; // = 0.45;

    Data(float _speedKp, float _speedKi, float _pitchKp, float _pitchKd) {
        speedKp = _speedKp;
        speedKi = _speedKi;
        pitchKp = _pitchKp;
        pitchKd = _pitchKd;
    }

    void printHeader() {
        // Serial.print("| acc pitch ");
        // Serial.print("| pitch");
        // Serial.print("| frequency");
        // Serial.println();
    }

    void print() {
        static long count;
        count++;
        if (count%4==0) {
            Serial.print("# ");
            Serial.print(desiredSpeed); Serial.print(" ");
            Serial.print(desiredDirection); Serial.print(" ");
            Serial.print(speedKp, 4); Serial.print(" ");
            Serial.print(speedKi, 4); Serial.print(" ");
            Serial.print(pitchKp, 4); Serial.print(" ");
            Serial.print(pitchKd, 4); Serial.print(" ");
            
            Serial.print(axesAccel.pitch, 2); Serial.print(" ");
            Serial.print(axes.pitch, 3); Serial.print(" ");
            Serial.print(desiredPitch, 3); Serial.print(" ");
            Serial.print(frequency, 3); Serial.print(" ");
            Serial.print(stepsPerSecond); Serial.print(" ");
            Serial.print(stepsPerSecondKp); Serial.print(" ");
            Serial.print(stepsPerSecondKd); Serial.print(" ");
            Serial.println("");
        }
        // Serial.print(va_arg(axesAccel.pitch,float));
        // // printValue(axes.pitch);
        // // printValue(frequency);
        // Serial.println();
        
    }

};




#endif

