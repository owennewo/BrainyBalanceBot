#include <Arduino.h>
#include <IMU.h>
#include <Motor.h>
#include <PID.h>
#include <Data.h>

#define MAX_SPEED 1200
#define MAX_ANGLE 25

#define MAX_ACCEL 20

const int fallThreshold = MAX_ANGLE; // give up if robot is more than this many degrees from vertical

long lastUpdateTime;
long lastSteerTime;
long lastPrintTimeMs;

Data data = Data(0.0027, 0.01, 200, 0.5);// = &data;
// Data data = Data(0.0027, 0.01, 0.06, 0.5);// = &data;

float count = 0.0;

void setup() {

  Serial.begin(500000);
  // while(!Serial) {}

  delay(3000);

  MotorDisable();
  // Serial.println("Starting");

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  ImuBegin();
  MotorInit();
  digitalWrite(13, LOW); // turn off LED while calibrating
  ImuCalibrate();
  digitalWrite(13, HIGH);
} 

// smooth out data readings
float smooth(float newValue, float oldValue, float alpha) {
  return alpha * oldValue + (1 - alpha) * newValue;
}

void processCommand(char commandCode, char * commandValue) {

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
        data.angleKp = atof(commandValue);
        // Serial.println(data.angleKp);
        break;
      // case 'p':
      //   data.angleKp -= 1;
      //   break;
      case 'D':
        data.angleKd = atof(commandValue);
        break;
      // case 'd':
      //   data.angleKd -= 0.025;
      //   break;
      case 'p':
        data.speedKp =atof(commandValue);
        break;
      // case 's':
      //   data.speedKp -= 0.001;
      //   break;
      case 'l':
        data.log = boolean(atoi(commandValue));
        break;
      // case 's':
      //   data.speedKp -= 0.001;
      //   break;
      case 'i':
        data.speedKi = atof(commandValue);
        break;
      case 'A':
        break;
  }
}

char command[30] = "\n";
char commandName;
char * commandValue;
byte commandIndex = 0;

void readSerial() {
  

  while (Serial.available()) {
    char received = Serial.read();
    command[commandIndex] = received; 
    
    // Serial.print(commandIndex);
    // Serial.print(":");
    // Serial.print(received);
    // Serial.print(":");
    // Serial.println((int) received);
    commandIndex++;
    // Process message when new line character is recieved
    if (received == '\n')
    {
        // Serial.println("Arduino Received: ");
        // Serial.println(command);
        commandName = strtok(command, " ")[0]; // must be single char please!
        if (commandName != '\0') {
          commandValue = strtok(NULL, "\n");
          if (commandName != '\0') {
            Serial.print((char) commandName);
            Serial.print(":");
            Serial.print((int) commandName);
            Serial.print(":");
            Serial.println(commandValue);
            Serial.println(command);
            processCommand(commandName, commandValue);
          } else {
            Serial.print("Unknown commandValue: ");
            Serial.println(command);
          }
        } else {
          Serial.print("Unknown commandName: ");
          Serial.println(command);
        }
        commandIndex = 0;    
    }
  }
  
}

bool fallen(float angle) {
  return angle > fallThreshold || angle < - fallThreshold;
}

void loop2() {

  readSerial();

  if (ImuRead()) {
    static long count;
    count++;
    if (count %100 == 0) {
      Serial.println(data.axes.roll);
    }
  }
}

void loop() {

  static boolean enabled;

  readSerial();
  
  lastUpdateTime = micros();
  if (ImuRead()) {
      float angle = data.axes.roll;
      
        if (fallen(angle)) {
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
            data.desiredAngle = PidSpeedToAngle(data.stepsPerSecond, data.desiredSpeed, data.frequency);
            // data.desiredAngle = 0;
            data.stepsPerSecond = PidAngleToSteps(angle, data.desiredAngle, data.frequency);
            // data.stepsPerSecond = stepsPerSecond;
        }

        // Serial.println(stepsPerSecond);
        static long count;
        count++;

        if (count%50 == 0) {
            // Serial.print("\rPITCH: ");
            // Serial.print(angle);
            // Serial.print(" SPEED: ");
            // Serial.print(speed);
            //  Serial.print(" SETSPEED: ");
            // Serial.print(setSpeed);           
            // Serial.print(" SETPITCH: ");
            // Serial.print(setAngle);
            // Serial.print(" STXXPS: ");
            // Serial.print(stepsPerSecond);    
        }

        data.stepsPerSecondLeft = data.stepsCountLeft * data.frequency / 32;
        data.stepsPerSecondRight = data.stepsCountRight * data.frequency / 32;
        data.stepsCountRight = 0;
        data.stepsCountLeft = 0;
        
        data.print();
        data.stepsPerSecond = constrain(data.stepsPerSecond, data.stepsPerSecondOld - MAX_ACCEL, data.stepsPerSecondOld + MAX_ACCEL);
        data.stepsPerSecond = constrain(data.stepsPerSecond, -MAX_SPEED, MAX_SPEED);

        // data.stepsPerSecond = smooth(data.stepsPerSecond, data.stepsPerSecondOld, 0.2);

        data.stepsPerSecondOld = data.stepsPerSecond;
        

        data.stepsPerSecondLeftDesired = (data.stepsPerSecond - data.desiredDirection);
        data.stepsPerSecondRightDesired = (data.stepsPerSecond + data.desiredDirection);
        MotorSetLeftSpeed(2 * (data.stepsPerSecond - data.desiredDirection));
        MotorSetRightSpeed(2 * (data.stepsPerSecond + data.desiredDirection));
  }
  
}


