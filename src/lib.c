/* Library File */
/* NOTE: Put function names in header file */
#include <lib.h>

/* void digitalWrite(int pin, int state){
    code
} */

void digitalWrite(int pin, int state){ // Can act as pullup enabler
    if (pin <= 7){
        if(state == 0){
            PORTD&=~(1<<pin);
        }
        else{
            PORTD|=(1<<pin);
        }
    }
    else{
        if (pin <= 13){
            if (state == 0){
                PORTB&=~(1<<(pin-8));
            }
            else{
                PORTB|=(1<<(pin-8));
            }
        }
    }
}

int digitalRead(int pin){
    int state;
    if (pin <= 7){
        state = ((PIND & (1<<pin))>>pin);
    }
    else{
        if (pin <= 13){
            state = ((PIND & (1<<(pin-8)))>>(pin-8));
        }
    }
    return state;
}

int analogRead(int pin){
    ADMUX&=(0xF0 + pin); // Select ADMUX for pin
    ADMUX|=(pin);
    ADCSRA|=(1<<ADSC); // ADC start conversion
    while(((ADCSRA & (1<<ADSC))>>ADSC) == 1){
        // Wait until conversion is finished
    }
    return ADC;
}

void enableDigitalPin(int pin, int state){ // Note: 1 is output, 0 is input
    if (pin <= 7){
        if (state == 0){
            DDRD&=~(1<<pin);
        }
        else{
            DDRD|=(1<<pin);
        }
    }
    else{
        if (pin <= 13){
            if (state == 0){
                DDRB&=~(1<<(pin-8));
            }
            else{
                DDRB|=(1<<(pin-8));
            }
        }
    }
}

void enableAnalogPin(int pin, int state){ // Note: 1 is output, 0 is input
    if (pin <= 5){
        if (state == 0){
            DDRC&=~(1<<pin);
        }
        else{
            DDRC|=(1<<pin);
        }
    }
}

void enableADC(int pin, int prescaler, int autoTrigger){ // Prescaler is 1 (counts as 2), 2, 4, 8, 16, 32, 64, 128. Voltage reference is default vcc
    if (prescaler == 1){
        ADCSRA&=~((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); // Set ADC prescaler to /128
    }
    if (prescaler == 2){
        ADCSRA|=(1<<ADPS0); // Set ADC prescaler to /128
        ADCSRA&=~((1<<ADPS2)|(1<<ADPS1)); // Set ADC prescaler to /128
    }
    if (prescaler == 4){
        ADCSRA|=(1<<ADPS1); // Set ADC prescaler to /128
        ADCSRA&=~((1<<ADPS2)|(1<<ADPS0)); // Set ADC prescaler to /128
    }
    if (prescaler == 8){
        ADCSRA|=((1<<ADPS1)|(1<<ADPS0)); // Set ADC prescaler to /128
        ADCSRA&=~(1<<ADPS2); // Set ADC prescaler to /128
    }
    if (prescaler == 16){
        ADCSRA|=(1<<ADPS2); // Set ADC prescaler to /128
        ADCSRA&=~((1<<ADPS1)|(1<<ADPS0)); // Set ADC prescaler to /128
    }
    if (prescaler == 32){
        ADCSRA|=((1<<ADPS2)|(1<<ADPS0)); // Set ADC prescaler to /128
        ADCSRA&=~(1<<ADPS1); // Set ADC prescaler to /128
    }
    if (prescaler == 64){
        ADCSRA|=((1<<ADPS2)|(1<<ADPS1)); // Set ADC prescaler to /128
        ADCSRA&=~(1<<ADPS0); // Set ADC prescaler to /128
    }
    if (prescaler == 128){
        ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); // Set ADC prescaler to /128
    }
    DDRC&=~(1<<pin); // Initialise analog pin as input
    ADMUX&=(0xF0 + pin); // Select ADMUX for pin
    ADMUX|=(pin);
    ADCSRA|=(1<<ADEN);
    ADCSRA|=(1<<ADSC); // ADC start conversion
    if (autoTrigger == 1){
        ADCSRA|=(1<<ADATE);
    }
    else{
        ADCSRA&=~(1<<ADATE);
    }
    ADMUX&=~(1<<REFS1); // Select internal vcc reference
    ADMUX|=(1<<REFS0);
}

void enableTimer(int timerNo, int prescaler, int startTime){ // Prescaler is 1, 8, 32(Tim2 only), 64, 128(Tim2 only), 256, or 1024
    if (timerNo == 0){
        PRR&=~(1<<PRTIM0); // Enable timer 0
        TCCR0A&=~(1<<WGM01|1<<WGM00); // Set Normal operation of timer 0
        TCCR0B&=~(1<<WGM02);
        if (prescaler == 1){
            TCCR0B|=(1<<CS00);
            TCCR0B&=~((1<<CS02)|(1<<CS01));
        }
        if (prescaler == 8){
            TCCR0B|=(1<<CS01);
            TCCR0B&=~((1<<CS02)|(1<<CS00));
        }
        if (prescaler == 64){
            TCCR0B|=((1<<CS01)|(1<<CS00));
            TCCR0B&=~(1<<CS02);
        }
        if (prescaler == 256){
            TCCR0B|=(1<<CS02);
            TCCR0B&=~((1<<CS01)|(1<<CS00));
        }
        if (prescaler == 1024){
            TCCR0B|=((1<<CS02)|(1<<CS00));
            TCCR0B&=~(1<<CS01);
        }
        TCNT0 = startTime;
    }
    if (timerNo == 1){
        PRR&=~(1<<PRTIM1); // Enable timer 1
        TCCR1A&=~(1<<WGM11|1<<WGM10); // Set Normal operation of timer 1
        TCCR1B&=~((1<<WGM13)|(1<<WGM12));
        if (prescaler == 1){
            TCCR1B|=(1<<CS10);
            TCCR1B&=~((1<<CS12)|(1<<CS11));
        }
        if (prescaler == 8){
            TCCR1B|=(1<<CS11);
            TCCR1B&=~((1<<CS12)|(1<<CS10));
        }
        if (prescaler == 64){
            TCCR1B|=((1<<CS11)|(1<<CS10));
            TCCR1B&=~(1<<CS12);
        }
        if (prescaler == 256){
            TCCR1B|=(1<<CS12);
            TCCR1B&=~((1<<CS11)|(1<<CS10));
        }
        if (prescaler == 1024){
            TCCR1B|=((1<<CS12)|(1<<CS10));
            TCCR1B&=~(1<<CS11);
        }
        TCNT1 = startTime;
    }
    if (timerNo == 2){
        PRR&=~(1<<PRTIM2); // Enable timer 2
        TCCR2A&=~(1<<WGM21|1<<WGM20); // Set Normal operation of timer 2
        TCCR2B&=~(1<<WGM22);
        if (prescaler == 1){
            TCCR1B|=(1<<CS20);
            TCCR1B&=~((1<<CS22)|(1<<CS21));
        }
        if (prescaler == 8){
            TCCR1B|=(1<<CS21);
            TCCR1B&=~((1<<CS22)|(1<<CS20));
        }
        if (prescaler == 32){
            TCCR1B|=((1<<CS21)|(1<<CS20));
            TCCR1B&=~(1<<CS22);
        }
        if (prescaler == 64){
            TCCR1B|=(1<<CS22);
            TCCR1B&=~((1<<CS21)|(1<<CS20));
        }
        if (prescaler == 128){
            TCCR1B|=((1<<CS22)|(1<<CS20));
            TCCR1B&=~(1<<CS21);
        }
        if (prescaler == 256){
            TCCR1B|=((1<<CS22)|(1<<CS21));
            TCCR1B&=~(1<<CS20);
        }
        if (prescaler == 1024){
            TCCR1B|=((1<<CS22)|(1<<CS21)|(1<<CS20));
        }
        TCNT2 = startTime;
    }
}

void enableTimerInterrupt(int timerNo, int compareMatchReg){ // NOTE: compareMatchReg 0 is A, 1 is B
    if (timerNo == 0){
        if (compareMatchReg == 0){
            TIMSK0|=(1<<OCIE0A);
        }
        else{
            TIMSK0|=(1<<OCIE0B);
        }
    }
    if (timerNo == 1){
        if (compareMatchReg == 0){
            TIMSK1|=(1<<OCIE1A);
        }
        else{
            TIMSK1|=(1<<OCIE1B);
        }
    }
    if (timerNo == 2){
        if (compareMatchReg == 0){
            TIMSK2|=(1<<OCIE2A);
        }
        else{
            TIMSK2|=(1<<OCIE2B);
        }
    }
}

void enableTimerOverflow(int timerNo){ // NOTE: compareMatchReg 0 is A, 1 is B
    if (timerNo == 0){
        TIMSK0|=(1<<TOIE0);
    }
    if (timerNo == 1){
        TIMSK1|=(1<<TOIE1);
    }
    if (timerNo == 2){
        TIMSK2|=(1<<TOIE2);
    }
}

void delay(int activeTimer, unsigned int delayLength){ //NOTE: delayLength is in ticks based on timer prescaler etc.
    if (activeTimer == 0){
        unsigned int delay = TCNT0 + delayLength;
        while(delay < TCNT0){
        }
        while(TCNT0 < delay){
        }
    }
    if (activeTimer == 1){
        unsigned int delay = TCNT1 + delayLength;
        while(delay < TCNT1){
        }
        while(TCNT1 < delay){
        }
    }
    if (activeTimer == 2){
        unsigned int delay = TCNT2 + delayLength;
        while(delay < TCNT2){
        }
        while(TCNT2 < delay){
        }
    }
}

void digitalToggle(int pin){
    if (pin <= 7){
        PORTD^=(1<<pin);
    }
    else{
        PORTB^=(1<<(pin-8));
    }
}

void enableExternalInterrupt(int number, int type){ // 0 is on low, 1 is any change, 2 is falling edge, 3 is rising edge
  if(number==0){
    DDRD&=~(1<<DDD2);
    PORTD|=(1<<PORTD2);
    EIMSK|=(1<<INT0);
    if(type==0){
      EICRA&=~((1>>ISC01)|(1<<ISC00));
    }
    if(type==1){
      EICRA&=~(1<<ISC01);
      EICRA|=(1<<ISC00);
    }
    if(type==2){
      EICRA|=(1<<ISC01);
      EICRA&=~(1<<ISC00);
    }
    if(type==3){
      EICRA|=((1<<ISC01)|(1<<ISC00));
    }
  }
  if(number==1){
    DDRD&=~(1<<DDD3);
    PORTD|=(1<<PORTD3);
    EIMSK|=(1<<INT1);
    if(type==0){
      EICRA&=~((1>>ISC11)|(1<<ISC10));
    }
    if(type==1){
      EICRA&=~(1<<ISC11);
      EICRA|=(1<<ISC10);
    }
    if(type==2){
      EICRA|=(1<<ISC11);
      EICRA&=~(1<<ISC10);
    }
    if(type==3){
      EICRA|=((1<<ISC11)|(1<<ISC10));
    }
  }
}

void lcdInitI2C(){
  TWCR = (1<<TWINT)|(1<<TWEN) | (1<<TWSTO);
  TWBR = 0x48;
  int a;
  int SLA_W = 0x4E;
  int START = 0x08;
  int MT_SLA_ACK = 0x18;
  int MT_DATA_ACK = 0x28;
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // Start Bit
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != START){
    // ERROR();
  }
  for(a = 0; a<=192; a++);
  TWDR = SLA_W; // Slave address
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_SLA_ACK){
    // ERROR2();
  }
  for(a = 0; a<=192; a++);
  // Set to 4 bit, 2 line
  TWDR = 0x28; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x2C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x28; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=192; a++);
  TWDR = 0x28; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x2C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x28; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=192; a++);
  TWDR = 0x88; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x8C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x88; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=62000; a++);
  // Set cursor on, blinking
  TWDR = 0x00; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x04; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x00; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=192; a++);
  TWDR = 0xF0; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0xF4; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0xF0; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=62000; a++);
  // Return Cursor to Home
  TWDR = 0x08; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x0C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x08; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=192; a++);
  TWDR = 0x28; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x2C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x28; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=62000; a++);
}

void lcdClearI2C(){
  int MT_DATA_ACK = 0x28;
  int a;
  TWDR = 0x08; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x0C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x08; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=192; a++);
  TWDR = 0x18; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x1C; // Send 4bit text part 1
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  TWDR = 0x18; // Send 4bit text part 1, finishing E pulse
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK){
    ERROR3();
  }
  for(a = 0; a<=62000; a++);
}

void lcdWriteI2C(uint8_t ascii[]){
  int i=0;
  int MT_DATA_ACK = 0x28;
  while(i<=14){
    int a;
    uint8_t currentASCII = ascii[i];
    uint8_t leftSigASCII = (currentASCII & 0xF0);
    uint8_t leftNotSigASCII = ((currentASCII & 0x0F) << 4);
    TWDR = (leftSigASCII | 0x09); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (leftSigASCII | 0x0D); // Send 4bit text part 1
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (leftSigASCII | 0x09); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    for(a = 0; a<=192; a++);
    TWDR = (leftNotSigASCII | 0x09); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (leftNotSigASCII | 0x0D); // Send 4bit text part 1
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (leftNotSigASCII | 0x09); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    for(a = 0; a<=192; a++);
    i = i + 1;
  }
}


void ERROR(){
  DDRD|=(1<<DDD0);
  while(1){
    PORTD^=(1<<PORTD0);
    for(int a; a<=62000; a++);
  }
}

void ERROR2(){
  DDRD|=(1<<DDD1);
  while(1){
    PORTD^=(1<<PORTD1);
    for(int a; a<=62000; a++);
  }
}

void ERROR3(){
  DDRD|=(1<<DDD0);
  DDRD|=(1<<DDD1);
  PORTD|=(1<<PORTD0);
  PORTD&=~(1<<PORTD1);
  while(1){
    PORTD^=(1<<PORTD0);
    PORTD^=(1<<PORTD1);
    for(int a; a<=62000; a++);
  }
}

void lcdGotoI2C(unsigned int address){
  int a;
  int MT_DATA_ACK = 0x28;
  uint8_t leftAddress = ((address) & 0xF0);
  uint8_t rightAddress = (((address) & 0x0F) << 4);
    TWDR = (leftAddress | 0x88); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (leftAddress | 0x8C); // Send 4bit text part 1
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (leftAddress | 0x88); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    for(a = 0; a<=192; a++);
    TWDR = (rightAddress | 0x88); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (rightAddress | 0x8C); // Send 4bit text part 1
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (rightAddress | 0x88); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    for(a = 0; a<=192; a++);
}

void lcdHomeI2C(){
  int a;
  int MT_DATA_ACK = 0x28;
    TWDR = (0x08); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (0x0C); // Send 4bit text part 1
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (0x08); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    for(a = 0; a<=192; a++);
    TWDR = (0x28); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (0x2C); // Send 4bit text part 1
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    TWDR = (0x28); // Send 4bit text part 1, finishing E pulse
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != MT_DATA_ACK){
      ERROR3();
    }
    for(a = 0; a<=62500; a++);
}

