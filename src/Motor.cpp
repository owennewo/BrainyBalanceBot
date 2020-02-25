#include <Arduino.h>

#define TIMER_PRESCALER_DIV 16


// #define TIMER_FREQ 1000000

const int microsteps = 16; //8; // A4988 is set for 1/8th steps

// const int microstepPin1 = 6;
// const int microstepPin2 = 7;
// const int microstepPin3 = 8;
const int rightDirPin  = 3;
const int rightStepPin = 6;
const int leftDirPin   = 2;
const int leftStepPin  = 5;
const int enablePin  = 8;


TcCount16* _tcMotorLeft = (TcCount16*) TC4;
TcCount16* _tcMotorRight = (TcCount16*) TC5;


void leftStep() {
  static boolean toggle;
  toggle = !toggle; 
  digitalWrite(leftStepPin, toggle);
//   digitalWrite(leftStepPin, LOW);
}

void rightStep() {
  static boolean toggle;
  toggle = !toggle;   
  digitalWrite(rightStepPin, toggle);
//   digitalWrite(rightStepPin, LOW);
}

void TC4_Handler() {
    TcCount16* TC = (TcCount16*) TC4;
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;

      rightStep();
  }
}

void TC5_Handler() {

    TcCount16* TC = (TcCount16*) TC5;
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    leftStep();
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
    digitalWrite(enablePin, LOW);
  
}

void MotorDisable() {
  digitalWrite(enablePin, HIGH);  
}

void MotorInit() {
//   pinMode(microstepPin1, OUTPUT);
//   pinMode(microstepPin2, OUTPUT);
//   pinMode(microstepPin3, OUTPUT);
  
  pinMode(rightDirPin, OUTPUT);
  pinMode(rightStepPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  pinMode(leftStepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  // disable motors initially
  digitalWrite(enablePin, HIGH);
  
  startTimer(_tcMotorLeft,(long) 1);
  startTimer(_tcMotorRight,(long) 1);
  
//   Timer1.initialize(TIMER_FREQ);
//   Timer1.attachInterrupt(leftStep);
//   Timer3.initialize(TIMER_FREQ);
//   Timer3.attachInterrupt(rightStep);

//   setMicrosteps(microsteps);
}

void setDirection(int pin, long stepsPerSecond) {
  if (stepsPerSecond >= 0) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}


void MotorSetLeftSpeed(long stepsPerSecond) {
  setTimerFrequency(_tcMotorRight, abs(stepsPerSecond * microsteps));
//   Timer1.setPeriod(TIMER_FREQ / abs(stepsPerSecond * microsteps));
  setDirection(leftDirPin, stepsPerSecond);
}

void MotorSetRightSpeed(long stepsPerSecond) {
  setTimerFrequency(_tcMotorLeft, abs(stepsPerSecond * microsteps));
//   Timer3.setPeriod(TIMER_FREQ / abs(stepsPerSecond * microsteps));
  setDirection(rightDirPin, stepsPerSecond);
}




// void setMicrosteps(int microsteps) {
//   switch(microsteps) {
//     case 1:
//       digitalWrite(microstepPin1, LOW);
//       digitalWrite(microstepPin2, LOW);
//       digitalWrite(microstepPin3, LOW);
//       break;
//     case 2:
//       digitalWrite(microstepPin1, HIGH);
//       digitalWrite(microstepPin2, LOW);
//       digitalWrite(microstepPin3, LOW);
//       break;
//     case 4:
//       digitalWrite(microstepPin1, LOW);
//       digitalWrite(microstepPin2, HIGH);
//       digitalWrite(microstepPin3, LOW);
//       break;
//     case 8:
//       digitalWrite(microstepPin1, HIGH);
//       digitalWrite(microstepPin2, HIGH);
//       digitalWrite(microstepPin3, LOW);
//       break;
//     case 16:
//       digitalWrite(microstepPin1, HIGH);
//       digitalWrite(microstepPin2, HIGH);
//       digitalWrite(microstepPin3, HIGH);
//       break;
//   }
// }
