#include <Arduino.h>
#include <Data.h>

#define TIMER_PRESCALER_DIV 16

extern Data data;

const int microsteps = 16; //8; // A4988 is set for 1/8th steps

const int rightDirPin  = 3;
const int rightStepPin = 6;
const int leftDirPin   = 2;
const int leftStepPin  = 5;
const int enablePin  = 8;

long oldLeftSpeed = 0;
long oldRightSpeed = 0;


TcCount16* _tcMotorLeft = (TcCount16*) TC4;
TcCount16* _tcMotorRight = (TcCount16*) TC5;


void leftStep() {
  static boolean toggle;
  data.stepsCountLeft++;
  toggle = !toggle; 
  digitalWrite(leftStepPin, toggle);
//   digitalWrite(leftStepPin, LOW);
}

void rightStep() {
  static boolean toggle;
  data.stepsCountRight++;
  toggle = !toggle;   
  digitalWrite(rightStepPin, toggle);
//   digitalWrite(rightStepPin, LOW);
}

void TC4_Handler() {
    TcCount16* TC = (TcCount16*) TC4;
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
      leftStep();
  }
}

void TC5_Handler() {

    TcCount16* TC = (TcCount16*) TC5;
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    rightStep();
  }
}

void setTimerFrequency(TcCount16* TC, long frequencyHz) {

  if (frequencyHz == 0) {
    // todo perhaps its better to disable the timer - this might happen a lot though?
    frequencyHz = 1;
  }

  int compareValue = (SystemCoreClock / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  
  while (TC->STATUS.bit.SYNCBUSY == 1);
  
}

void startTimer(TcCount16* TC, long frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC4_TC5) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync


  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Set prescaler to 16
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  setTimerFrequency(TC, frequencyHz);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;
  
  if (TC == (TcCount16*) TC4) {
    NVIC_EnableIRQ(TC4_IRQn);
  } else if (TC == (TcCount16*) TC5) {
    NVIC_EnableIRQ(TC5_IRQn);
  }

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}


void MotorEnable() {
  Serial.println("MOTOR +");
    digitalWrite(enablePin, LOW);
}

void MotorDisable() {
  Serial.println("MOTOR -");
  digitalWrite(enablePin, HIGH);  
}

void MotorInit() {
  
  pinMode(rightDirPin, OUTPUT);
  pinMode(rightStepPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  pinMode(leftStepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  // disable motors initially
  digitalWrite(enablePin, HIGH);
  
  startTimer(_tcMotorLeft,(long) 1);
  startTimer(_tcMotorRight,(long) 1);
  
}

void setDirection(int pin, long stepsPerSecond) {
  if (stepsPerSecond >= 0) {
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, HIGH);
  }
}

void MotorSetLeftSpeed(long stepsPerSecond) {
  setTimerFrequency(_tcMotorLeft, abs(stepsPerSecond * microsteps));
  setDirection(leftDirPin, stepsPerSecond);
}

void MotorSetRightSpeed(long stepsPerSecond) {
  setTimerFrequency(_tcMotorRight, abs(stepsPerSecond * microsteps));
  setDirection(rightDirPin, stepsPerSecond);
}
