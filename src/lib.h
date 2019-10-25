#include <avr/io.h>
#include <avr/interrupt.h>

/* write function names */
// void digitalWrite(int pin, int state);
void digitalWrite(int pin, int state);
int digitalRead(int pin);
int analogRead(int pin);
void enableDigitalPin(int pin, int state);
void enableAnalogPin(int pin, int state);
void enableADC(int pin, int prescaler, int autoTrigger);
void enableTimer(int timerNo, int prescaler, int startTime);
void enableTimerInterrupt(int timerNo, int compareMatchReg);
void enableTimerOverflow(int timerNo);
void delay(int activeTimer, unsigned int delayLength);
void digitalToggle(int pin);
void enableExternalInterrupt(int number, int type);
void lcdInitI2C();
void lcdClearI2C();
void lcdWriteI2C(uint8_t ascii[]);
void ERROR();
void ERROR2();
void ERROR3();
void lcdGotoI2C(unsigned int address);
void lcdHomeI2C();
