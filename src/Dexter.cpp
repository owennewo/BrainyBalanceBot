#include <Arduino.h>
#include <IMU.h>
#include <Motor.h>
#include <PID.h>
#include <Data.h>
#include <Arduino_LSM6DS3.h>

// #define FREQUENCY 104 // number of motor updates per second

#define MAX_ANGLE 15
#define LOG_IMU false
#define LOG_SPEED_PID false
#define LOG_ANGLE_PID false

// float speedKp = 0.0027;
// float speedKi = 0; //0.0006;
// float pitchKp = 80;
// float pitchKd = 0.45;
const int fallThreshold = MAX_ANGLE; // give up if robot is more than this many degrees from vertical

// long setSpeed = 0;
// float setPitch = 0;
// long setDirection = 0;
float maxAccel = 1;
// float speed = 0;
// long stepsPerSecond;
// float differential = 0;

long lastUpdateTime;
long lastSteerTime;
long lastPrintTimeMs;

// Config data = Config

Data data = Data(0.0027, 0, 160, 0.5);// = &data;

float count = 0.0;

void setup() {
  Serial.begin(115200);
  while(!Serial) {}


  // data.printHeader();
  // data.print();
  // pinMode(13, OUTPUT);
  // digitalWrite(13, HIGH);
  // Serial.begin(57600);
  // Serial1.begin(38400);
  // ImuInit();
  // MotorInit();
  // delay(1000); // pause before starting IMU calibration
  // digitalWrite(13, LOW); // turn off LED while calibrating
  // ImuCalibrate();
  // digitalWrite(13, HIGH);



  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
  Serial.println("X\tY\tZ");

} 

void loop() {
  float x, y, z;

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);

    // Serial.print(x);
    // Serial.print('\t');
    Serial.println(y);
    // Serial.print('\t');
    // Serial.println(z);
  }
}

// smooth out data readings
float smooth(float newValue, float oldValue, float alpha) {
  return alpha * oldValue + (1 - alpha) * newValue;
}

void processCommand(char commandCode, char * commandValue) {
  // if (Serial.available()) {
  //   char key = Serial.read();
  //   Serial.println(key);
    // bool changed = true;
    switch(commandCode) {
       case 's':
        data.desiredSpeed = atof(commandValue);
        // lastSteerTime = millis();
        break;
      case 'd':
        data.desiredDirection = atoi(commandValue);
        // lastSteerTime = millis();
        break;
      case 'P':
        data.pitchKp = atof(commandValue);
        // Serial.println(data.pitchKp);
        break;
      // case 'p':
      //   data.pitchKp -= 1;
      //   break;
      case 'D':
        data.pitchKd = atof(commandValue);
        break;
      // case 'd':
      //   data.pitchKd -= 0.025;
      //   break;
      case 'p':
        data.speedKp =atof(commandValue);
        break;
      // case 's':
      //   data.speedKp -= 0.001;
      //   break;
      case 'i':
        data.speedKi = atof(commandValue);
        break;
      // case 'i':
      //   data.speedKi -= 0.0001;
      //   break;
      // case 't':
      //   Serial.println("twiddling");
      //   PidStartTwiddling();
      //   break;
      //   // 1 m/s = 800 steps/s
      //   setSpeed = smooth(Serial.parseFloat() * 800, setSpeed, 0.9);
      //   lastSteerTime = millis();
      //   break;
      case 'A':
        break;
        // 1 rad/s = 50 steps/s
      // Serial.print(" speedKi: ");
      // Serial.println(data.speedKi, 4);
      // Serial.print(" pitchKp: ");
      // Serial.println(data.pitchKp);
      // Serial.print(" pitchKd: ");
      // Serial.println(data.pitchKd, 3);
      // Serial.print(" Steps: ");
      // Serial.println(stepsPerSecond);
    
  }
}

char command[30];
char commandName;
char * commandValue;
byte commandIndex = 0;

void readSerial() {
  

  while (Serial.available()) {
    char recieved = Serial.read();
        command[commandIndex] = recieved; 
        commandIndex++;
        // Process message when new line character is recieved
        if (recieved == '\n')
        {
            Serial.println("Arduino Received: ");
            Serial.println(command);
            commandName = strtok(command, " ")[0]; // must be single char please!
            commandValue = strtok(NULL, "\n");
            Serial.println(commandName);
            Serial.println(commandValue);
            processCommand(commandName, commandValue);
            commandIndex = 0; 
            
        }

  }
  
}


bool fallen(float pitch) {
  return pitch > fallThreshold || pitch < - fallThreshold;
}



void loop2() {


//   if (micros() - lastUpdateTime < 1000000 / FREQUENCY) {
//     return;
//   }
  static boolean enabled;

  readSerial();
  
  lastUpdateTime = micros();
  if (ImuRead()) {
      float pitch = ImuCalculate();
      

        // if (lastSteerTime < millis() - BLUETOOTH_TIMEOUT) {
        //     // Serial.println("Timed out");
        //     setSpeed = smooth(0, setSpeed, 0.9);
        //     differential = smooth(0, differential, 0.9);
        // }

        // setSpeed = PidTwiddle(speed, pitch);
        
        if (fallen(pitch)) {
            if (enabled) {
                enabled = false;
                MotorDisable();
            }
            // Serial.println("Fallen");
            data.stepsPerSecond = 1;
        } else {
            if (!enabled) {
                enabled = true;
                MotorEnable();
            }
            // speed = smooth(stepsPerSecond, speed, 0.9);
            // speed = stepsPerSecond;
            data.desiredPitch = PidSpeedToPitch(data.stepsPerSecond, data.desiredSpeed, data.frequency);
            // data.desiredPitch = 0;
            data.stepsPerSecond = PidPitchToSteps(pitch, data.desiredPitch, data.frequency);
            // data.stepsPerSecond = stepsPerSecond;
        }

        // Serial.println(stepsPerSecond);
        static long count;
        count++;

        if (count%50 == 0) {
            // Serial.print("\rPITCH: ");
            // Serial.print(pitch);
            // Serial.print(" SPEED: ");
            // Serial.print(speed);
            //  Serial.print(" SETSPEED: ");
            // Serial.print(setSpeed);           
            // Serial.print(" SETPITCH: ");
            // Serial.print(setPitch);
            // Serial.print(" STXXPS: ");
            // Serial.print(stepsPerSecond);    
        }

        data.print();

        MotorSetLeftSpeed(data.stepsPerSecond - data.desiredDirection);
        MotorSetRightSpeed(data.stepsPerSecond + data.desiredDirection);
  }
  
}


