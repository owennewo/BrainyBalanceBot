#include <Arduino.h>
#include <Data.h>

char command[30] = "\n";
char commandName;
char * commandValue;
byte commandIndex = 0;

extern Data data;

void processCommand(char commandCode, char * commandValue) {

    switch(commandCode) {
       case 's':
        data.speedInput = atof(commandValue);
        // lastSteerTime = millis();
        break;
      case 'd':
        data.directionInput = atoi(commandValue);
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

void readSerial() {

  while (Serial.available()) {
    char received = Serial.read();
    command[commandIndex] = received; 
    
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
