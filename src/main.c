#include <avr/io.h>
#include <avr/interrupt.h>
#include <lib.h>

const uint16_t fuelIPS[200]={860,1083,1523,2126,2855,3667,4524,5384,6207,6936, // <500 RPM, 0-9 throttle position
1050,1271,1705,2302,3022,3826,4673,5523,6337,7058, // 500-1000 RPM, 0-9 throttle position 
1235,1453,1883,2474,3187,3982,4820,5661,6467,7180,// Idle 1000RPM, estimated 1.235ms pulsewidth
1250,1472,1910,2511,3237,4046,4899,5756,6576,7302,
1325,1549,1990,2596,3327,4143,5003,5866,6692,7424,
1400,1626,2070,2681,3418,4240,5106,5976,6809,7546,
1475,1703,2150,2766,3508,4336,5209,6086,6925,7668,
1550,1779,2230,2851,3599,4433,5313,6196,7041,7790,
1625,1856,2311,2935,3689,4530,5416,6306,7158,7912,
1700,1933,2391,3020,3780,4627,5519,6416,7274,8034,
1774,2007,2464,3093,3851,4696,5588,6483,7339,8097,
1850,2082,2538,3165,3922,4765,5655,6548,7403,8160,
1850,2082,2538,3165,3922,4765,5655,6548,7403,8160, // Peak horsepower estimate, 8.160ms
1850,2082,2538,3165,3922,4765,5655,6548,7403,8160,
1750,1983,2442,3073,3834,4683,5577,6475,7336,8097,
1650,1885,2346,2981,3746,4600,5499,6403,7268,8034,
1550,1784,2244,2876,3639,4489,5386,6287,7149,7912,
1450,1683,2141,2771,3531,4379,5273,6170,7029,7790,
1350,1582,2039,2667,3424,4269,5160,6054,6910,7668,
1350,1578,2026,2641,3384,4213,5086,5963,6803,7546}; // Fuel lookup for 20 RPM ranges and 10 throttle positions (IPS = Injector pulse width in nanoseconds (milliseconds * 1000))
uint8_t offsetAngle = 10; // Account for spark signal offset
const uint16_t fuelAdvanceAngle[20]={ 270, 270, 290, 310, 330, 350, 10, 30, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50}; // Fuel advance based on RPM. Use same value as to determine lookup table
const uint8_t sparkAdvanceAngle[20]={ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}; // SPARK ADVANCE ANGLE BASED ON RPM
uint16_t oldTTS;
uint16_t newTTS;
uint16_t expTTS;
uint16_t currentRPM;
uint8_t rpmLookup;
uint16_t sensor;
uint8_t sensorLookup;
uint8_t lookup;
uint16_t fuelStart;
uint16_t fuelOn;
uint16_t fuelEnd;
uint8_t systemState; // Note: 0 b setupComplete 0 0 0 0 sparkState engineRunning fuelState
uint8_t ovfCount;
uint8_t serialDelay;
uint8_t calc;
uint8_t writeRow1[15] = {0x52, 0x50, 0x4D, 0x3A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
uint8_t writeRow2[15] = {0x4E, 0x6F, 0x74, 0x20, 0x52, 0x75, 0x6E, 0x6E, 0x69, 0x6E, 0x67, 0x20, 0x20, 0x20, 0x20};


ISR(TIMER0_OVF_vect){
  if(ovfCount >= 20){
    oldTTS = newTTS; // Set oldTTS to newTTS value
    newTTS = TCNT1; // Set newTTS to current timer 1 value
    TCNT1 = 0; // Reset Timer 1
    systemState|= 0b00000010; // Set engineRunning to 1
    ovfCount = 0;
    calc = 0;
    digitalToggle(4);
  }
  ovfCount = (ovfCount + 1);
}

ISR(TIMER1_COMPA_vect){ // On Compare match (FUEL)
  digitalToggle(5); // Turn fuel on (or off)
  systemState^= 0b00000001; // Toggle fuelState
  digitalToggle(4);
}

ISR(TIMER1_COMPB_vect){ // On Compare match (SPARK)
  digitalToggle(6);
  systemState^= 0b00000100; // Toggle sparkState
}

ISR(TIMER1_OVF_vect){
  systemState &= ~(0b00000010); // Turn off engineRunning bit
}

/* ISR(TIMER2_OVF_vect){
  if(serialDelay == 0){
    lcdHomeI2C(); // Send cursor to home position
  }
  if(serialDelay == 10){
    lcdWriteI2C(writeRow1); // Write row 1
  }
  if(serialDelay == 20){
    lcdGotoI2C(0x38);  // Send cursor to start of second row
  }
  if(serialDelay == 30){
    lcdWriteI2C(writeRow2);
  }
  if(serialDelay < 30){
    serialDelay = (serialDelay + 1);
  }
  if(serialDelay >= 30){
    serialDelay = 0;
  }
} */

void setup(){
  // Set starting variable values
  systemState = 0x00;
  serialDelay = 0;
  // Enable interrupts
  // Set up interrupt on pin 2
  enableDigitalPin(2, 0); // Enable digital pin 2 as input for interrupt
  // enableExternalInterrupt(0, 3); // Enable external interrupt on pin 2 as rising edge
  digitalWrite(2, 0); // Disable internal pullup on pin 2
  // Set up analog input on pin A0
  enableAnalogPin(0, 0); // Enable pin A0 as input
  enableADC(0, 128, 0); // Enable ADC for pin A0 with prescaler 128 and no autotrigger
  // Set up general I/O
  enableDigitalPin(4, 1); // Enable digital pin 4 for testing
  enableDigitalPin(5, 1); // Enable digital pin 5 as output for fuel
  digitalWrite(5, 0); // Turn off pin 5
  enableDigitalPin(6, 1); // Enable digital Pin 6 as output for spark
  digitalWrite(6, 0); // Turn off pin 6
  enableDigitalPin(7, 1); // Enable digital pin 7 as output for engine run state
  digitalWrite(7, 0); // Turn off pin 7
  // enableAnalogPin(4, 0);
  enableAnalogPin(5, 0);
  // Enable timers and timer interrupts
  enableTimer(0, 256, 0); // Enable Timer 0
  enableTimerOverflow(0); // Enable Timer 0 overflow for simulating spark input
  enableTimer(1, 256, 0); // Enable timer 1
  enableTimerOverflow(1); // Enable timer 1 overflow for time out
  enableTimerInterrupt(1, 0); // Enable Timer 1 compare match register A
  enableTimerInterrupt(1, 1); // Enable Timer 1 compare match register B
  enableTimer(2, 1024, 0); // Enable Timer 2 for use in delays
  // enableTimerOverflow(2);
  // Initialise LCD screen
  /* lcdInitI2C(); // Initialise Screen
  delay(2, 100); // Delay for stability
  lcdClearI2C(); // LCD Clear
  delay(2, 100); // Delay for stability
  lcdHomeI2C(); // LCD return to home address
  delay(2, 100); // Delay for stability */
  sei();
  systemState |= 0b10000000; // Set setup completed bit
  
}

int main(void){
  if( (systemState & 0b10000000) == 0){ // If setup not yet completed
    setup();
  }
  while(1){
    if(calc == 0){
      expTTS = (newTTS + newTTS - oldTTS); // Calculate expected time to next interrupt
      if(expTTS <= 0){ // If expTTS is 0, don't do anything (avoids calculation error)
        currentRPM = 0;
      }
      else{
        currentRPM = (60 * (62500 / expTTS)); // Calculate current RPM
      }
      rpmLookup = (10 * (currentRPM / 500)); // Calculate rpm lookup value (x10)
      sensor = (analogRead(5)); // Read analog pin 0 for sensor value
      if(sensor >= 125){ // If sensor above a threshold
        sensorLookup = ((sensor - 25) / 100); // Sensor lookup value is 1 or greater
      } // END IF STATEMENT
      else{
        sensorLookup = 0; // Sensor lookup value is 0
      } // END ELSE STATEMENT
      lookup = (rpmLookup + sensorLookup);
      fuelStart = ((expTTS - (expTTS * (offsetAngle + fuelAdvanceAngle[lookup]))) / 360); // Calculate fuel start timing
      if(fuelStart > expTTS){ // If fuel starts after end of revolution
        fuelStart = fuelStart - expTTS; // Set expected fuel Start value
      } // END IF STATEMENT
      fuelOn = (0.0625 * fuelIPS[(rpmLookup + sensorLookup)]); // Calculate fuel duty cycle
      fuelEnd = (fuelStart + fuelOn); // Calculate fuel end time
      if(fuelEnd > expTTS){ // If fuel ends after end of revolution
        fuelEnd = fuelEnd - expTTS; // Set expected fuel end value
      } // END IF STATEMENT
      calc = 1;
    }
    if((systemState & 0b00000001) == 0){ // If fuelling not yet started
      OCR1A = fuelStart; // Set compare match A for fuel start
      
    } // END IF STATEMENT
    else{ // If fuelling started
      OCR1A = fuelEnd; // Set compare match A for fuel end
    } // END ELSE STATEMENT
    OCR1B = 2500;
    if((systemState & 0b00000010) == 0){ // If engine NOT RUNNING
      digitalWrite(7, 0); // Set engine running LED to ON
      // TCCR1B&=~((1<<CS12) | (1<<CS11) | (1<<CS10)); // STOP TIMER BY CLEARING PRESCALER BITS
      // TCNT1 = 0; // RESET TIMER TO 0
    }
    else{ // If engine not running
      digitalWrite(7, 1); // Set engine running LED to off
    }
  } // END FOREVER WHILE LOOP
} // END MAIN CODE