/**
   Latency measurement code -- measurement jig

   Simple tool to toggle a digital pin and measure the delay until
   the desired response happens. The function of the digital output,
   and the signal to be measured, are determined by whatever external
   hardware is attached.

   The resulting delay in microseconds is printed over the serial port
   so multiple trials can be made into a histogram. The granularity of
   the timer is 4us on standard 16MHz Arduino boards.

   (c) 2015 Andrew McPherson, C4DM, QMUL
   This is licensed under a Creative Commons Attribution-ShareAlike licence.

   updated version: 2019 Johnty Wang - IDMIL, McGill
   Addition of multiple trigger events: pin 3 is first input, pin 4 is second.
     Event T2 is now the time between T1 and when a serial byte is received
   
   
   To use: open serial port at 115200b
   
   T0 on UNO Pin 3: button to start measure
   T1 on UNO Pin 4: trigger from clapboard
   T2: triggered by receiving ANY serial data
      T2 can also be triggered on Pin 5, if you look at the gInputPin2PortReg bits 
      and comment out the Serial waiting bits for stage 2 of the timing.

   the output is 3 numbers, T0, T1, and T2, in microseconds. (note first number is a useless '0')...

*/

const int gOutputPin = 2;    // Pin sending the trigger

const int gInputPin0 = 3;     // Pin0 reading response t0
const int gInputPin1 = 4;     // Pin1 reading response t1
const int gInputPin2 = 5;     // Pin2 reading response t2

const int gLEDPin = 13;      // Pin for the LED on the board, for visual feedback

const int gDelayBetweenTrials = 150;  // Measured in milliseconds; only approximate

volatile uint8_t *gInputPin0PortReg, *gInputPin1PortReg, *gInputPin2PortReg, *gOutputPinPortReg;
uint8_t gInputPin0Bit, gInputPin1Bit, gInputPin2Bit, gOutputPinBit;

int gStage; //measurement stage: 0 = 1st stage, 1 = 2nd stage

void setup() {
  pinMode(gOutputPin, OUTPUT);
  pinMode(gInputPin0, INPUT);
  pinMode(gInputPin1, INPUT);
  pinMode(gLEDPin, OUTPUT);

  digitalWrite(gOutputPin, LOW);
  digitalWrite(gInputPin0, 0);
  digitalWrite(gInputPin1, 0);

  // Do some internal machinations to pull out the register for faster access
  // compared to digitalRead(). This is something of a violation of the abstractions
  // set up in the Arduino/Wiring environment, and might change on future boards.
  gInputPin0PortReg = portInputRegister(digitalPinToPort(gInputPin0));
  gInputPin1PortReg = portInputRegister(digitalPinToPort(gInputPin1));
  gInputPin2PortReg = portInputRegister(digitalPinToPort(gInputPin2));

  gOutputPinPortReg = portOutputRegister(digitalPinToPort(gOutputPin));

  gInputPin0Bit = digitalPinToBitMask(gInputPin0);
  gInputPin1Bit = digitalPinToBitMask(gInputPin1);
  gInputPin2Bit = digitalPinToBitMask(gInputPin2);

  gOutputPinBit = digitalPinToBitMask(gOutputPin);

  Serial.begin(115200);

  for (int i = 0; i < 10; i++) {
    digitalWrite(gLEDPin, HIGH);
    delay(50);
    digitalWrite(gLEDPin, LOW);
    delay(50);
  }
  delay(2000);
  //digitalWrite(gLEDPin, LOW);
  while (Serial.available())
    Serial.read();
}

void loop() {
  unsigned int overflows0 = 0;
  unsigned int finalCount0 = 0;

  unsigned int overflows1 = 0;
  unsigned int finalCount1 = 0;

  unsigned int overflows2 = 0;
  unsigned int finalCount2 = 0;

  gStage = 0; //start with stage 0

  // **** This code runs with interrupts off for predictable timing ****
  cli();

  // Prepare Timer1 initially stopped
  TCCR1A = 0;  // normal mode
  TCCR1B = 0;  // no clock source
  TCCR1C = 0;  // no force compare
  TIMSK1 = 0;  // no interrupts

  TIFR1 = 1;    // clear overflow bit
  TCNT1 = 0;   // count starts at 0


  // Toggle pin...
  // Equivalent (but slower): digitalWrite(gOutputPin, HIGH);
  //*gOutputPinPortReg |= gOutputPinBit;

  // ...and wait (possibly forever) for response
  // Equivalent (but slower): while(digitalRead(gInputPin0) != HIGH) { /* ,,, */ }

  // we wait here until we get the first trigger!
  if (gStage == 0) {
    digitalWrite(gLEDPin, HIGH);
    sei(); //unfortunately we have to do this, which will make timing less precise.
    // but we need it for the serial port...
    while (!(*gInputPin0PortReg & gInputPin0Bit)) {
      //      finalCount0 = TCNT1;  // Capture count
      //      if (TIFR1 & 1) {
      //        TIFR1 = 1;
      //        overflows0++;
      //      }
    }
    Serial.print(1);
    while ((*gInputPin0PortReg & gInputPin0Bit)) {
      //      finalCount0 = TCNT1;  // Capture count
      //      if (TIFR1 & 1) {
      //        TIFR1 = 1;
      //        overflows0++;
      //      }
      delay(5);
    }
    digitalWrite(gLEDPin, LOW);
  }

 
  
  while (Serial.available()) //clear serial buffer
    Serial.read();
  delay(5);

}
