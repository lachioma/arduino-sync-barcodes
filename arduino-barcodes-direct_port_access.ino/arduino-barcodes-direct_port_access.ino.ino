/*
  Digital barcodes for synchronizing data streams.
  Modified by Alessandro La Chioma ale.lach@gmail.com (2020-09)
  from code by Allen Institute https://github.com/open-ephys/sync-barcodes

  Each TTL barcode corresponds to a number expressed in binary that increment by 1 after each iteration.
  Immediately before each TTL barcode, an initiation pulse is given, consisting of a HIGH state followed by a LOW state of the same duration.
*/

// Pin settings:
int OUTPUT_PIN_1   = 25; //PD0
int OUTPUT_PIN_2   = 27; //PD2
int OUTPUT_PIN_3   = 29; //PD6
int OUTPUT_PIN_LED = 13; 
// Barcode settings:
int BARCODE_BITS = 12; // 2^BARCODE_BITS different barcodes (e.g. 2^8 = 256), with BARCODE_BITS (0/1) bars 
int INITIATION_PULSE = 100;   // ms. Duration of the HIGH state of initiation pulse. The initiation pulse consists of a HIGH state followed by a LOW state of the same duration, followed by the barcode.
int BARCODE_DURATION = 600;   // ms. Duration of entire barcode, excluding initial pulse. It must be a multiple of BARCODE_BITS
int BAR_DURATION = BARCODE_DURATION / BARCODE_BITS; // ms. Duration of each individual 1/0 bar.
int INTER_BARCODE_INTERVAL = 10000; // ms. Time interval between barcodes.
int delay_per_iteration = BAR_DURATION; // ms. Duration of each individual 1/0 bar.
int INITIAL_BARCODE_VALUE = 10; // First barcode value. Do not start from 0 because 0 has no HIGH states in binary; 10 in binary is 1010.
long barcode;

void setup() {
  
  pinMode(OUTPUT_PIN_1, OUTPUT); // initialize digital pin (this line is here only for code clarity, not necessary because we use direct port access)
  pinMode(OUTPUT_PIN_2, OUTPUT); // initialize digital pin (this line is here only for code clarity, not necessary because we use direct port access)
  pinMode(OUTPUT_PIN_3, OUTPUT); // initialize digital pin (this line is here only for code clarity, not necessary because we use direct port access)
  pinMode(OUTPUT_PIN_LED, OUTPUT); // initialize digital pin for on-board LED
  REG_PIOD_OWER = 0b00000000000000000000000001000101; // PIO_OWER PIO Output Write Enable Register - write 1's here to enable pins for writing
  REG_PIOD_OER  = 0b00000000000000000000000001000101; // PIO_OER  PIO Controller Output Enable Register - write 1's here to set pins as OUTPUT
  //randomSeed(analogRead(0));
  //barcode = random(1, pow(2,BARCODE_BITS));

  // Start from a given value and then increment by 1 each iteration. Once you reach the limit 2^BARCODE_BITS, it starts over.
  // Set the first barcode value (do not start from 0 because 0 has no HIGH states in binary):
  barcode = INITIAL_BARCODE_VALUE; 
  
}

void loop() {

  barcode += 1; // increment barcode on each cycle

//  digitalWrite(OUTPUT_PIN_1, HIGH);   // initialize with pulse
//  digitalWrite(OUTPUT_PIN_2, HIGH);   // initialize with pulse, delay +2.5 us
//  digitalWrite(OUTPUT_PIN_3, HIGH);   // initialize with pulse, delay +2.5 us
  REG_PIOD_ODSR = 0b00000000000000000000000001000101; // PIO_ODSR PIO Controller Output Data Status Register - write 1's here to set pins as HIGH
  digitalWrite(OUTPUT_PIN_LED, HIGH);   // initialize with pulse
  delay(INITIATION_PULSE);
//  digitalWrite(OUTPUT_PIN_1, LOW);    // set to low value for pulse duration
//  digitalWrite(OUTPUT_PIN_2, LOW);    // set to low value for pulse duration, delay +2.5 us
//  digitalWrite(OUTPUT_PIN_3, LOW);    // set to low value for pulse duration, delay +2.5 us
  REG_PIOD_ODSR = 0b00000000000000000000000000000000; // PIO_ODSR PIO Controller Output Data Status Register - write 1's here to set pins as HIGH
  digitalWrite(OUTPUT_PIN_LED, LOW);
  delay(INITIATION_PULSE);
  
  for (int i = 0; i < BARCODE_BITS; i++)
  {
    if ((barcode >> i) & 1) // takes i-th bit of barcode. The output of this condition is either 0 or 1 (https://playground.arduino.cc/Code/BitMath/)
    {
//      digitalWrite(OUTPUT_PIN_1,HIGH);
//      digitalWrite(OUTPUT_PIN_2,HIGH); // delay +2.5 us
//      digitalWrite(OUTPUT_PIN_3,HIGH); // delay +2.5 us
      REG_PIOD_ODSR = 0b00000000000000000000000001000101; // PIO_ODSR PIO Controller Output Data Status Register - write 1's here to set pins as HIGH
      digitalWrite(OUTPUT_PIN_LED, HIGH);
    }
    else
    {
//      digitalWrite(OUTPUT_PIN_1,LOW);
//      digitalWrite(OUTPUT_PIN_2,LOW); // delay +2.5 us
//      digitalWrite(OUTPUT_PIN_3,LOW); // delay +2.5 us
      REG_PIOD_ODSR = 0b00000000000000000000000000000000; // PIO_ODSR PIO Controller Output Data Status Register - write 1's here to set pins as HIGH
      digitalWrite(OUTPUT_PIN_LED, LOW);
    }

    
     delay(delay_per_iteration);
  }
  
//  digitalWrite(OUTPUT_PIN_1, LOW);       // write final low value
//  digitalWrite(OUTPUT_PIN_2, LOW);       // write final low value, delay +2.5 us
//  digitalWrite(OUTPUT_PIN_3, LOW);       // write final low value, delay +2.5 us
  REG_PIOD_ODSR = 0b00000000000000000000000000000000; // PIO_ODSR PIO Controller Output Data Status Register - write 1's here to set pins as HIGH
  digitalWrite(OUTPUT_PIN_LED, LOW);
  delay(INTER_BARCODE_INTERVAL);       // wait for interval

}
