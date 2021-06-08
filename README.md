# Arduino Sync Barcodes

This repository contains Arduino code (`arduino-barcodes`) for generating TTL barcodes that can be used to synchronize multiple data sources.


The code has been modified from [open-ephys/sync-barcodes](https://github.com/open-ephys/sync-barcodes).  
The modifications from the original code include:
- Simultaneous output on multiple pins using "direct port access" (see also section below).
- Barcode settings (barcode duration, inter-barcode interval, number of bits, initiation pulse duration, etc.) can now be easily changed to your preference.
- Several comments have been added for clarity.


Read more about synchronizing multiple data streams:  
<https://open-ephys.github.io/gui-docs/User-Manual/Plugins/NIDAQmx.html>



## More about Direct Port Access ("Port manipulation")

**N.B.** the way port manipulation is implemented depends on the exact microcontroller used by the board. In this case I used Arduino Due, whose microcontroller is SAM3X. With other boards things could work differently (e.g. it will certainly be different for ATmega8 and ATmega168). So what is written here <https://www.arduino.cc/en/Reference/PortManipulation> does not apply to Arduino Due!


If you use 
```
digitalWrite(OUTPUT_PIN_1, HIGH);
digitalWrite(OUTPUT_PIN_2, HIGH);
digitalWrite(OUTPUT_PIN_3, HIGH);
```
there will be about 2.5 us (microseconds) of delay between one output and the next one.

Synchronous output on multiple pins can be obtained using "port manipulation".  
Take the [pinout scheme](http://www.robgray.com/temp/Due-pinout-WEB.png) of the Arduino Due board. You must choose pin numbers that converge to the same port. For example, pins 25-30 on Arduino Due map onto port D.




To initialize the pins:
```
int OUTPUT_PIN_1   = 25; //PD0
int OUTPUT_PIN_2   = 27; //PD2
int OUTPUT_PIN_3   = 29; //PD6
```
```
void setup() {
  pinMode(OUTPUT_PIN_1, OUTPUT); // initialize digital pin (this line is here only for code clarity, not necessary because we use direct port access)
  pinMode(OUTPUT_PIN_2, OUTPUT); // initialize digital pin (this line is here only for code clarity, not necessary because we use direct port access)
  pinMode(OUTPUT_PIN_3, OUTPUT); // initialize digital pin (this line is here only for code clarity, not necessary because we use direct port access)
  REG_PIOD_OWER = 0b00000000000000000000000001000101; 
  REG_PIOD_OER  = 0b00000000000000000000000001000101;
```

`pinMode` lines are not needed, but we keep them just to make the code more readable.  
REG_PIOX_OWER and REG_PIOX_OER with X = A, B, C, D, E.  
PIO_OWER: PIO (Parallel Input Output) Output Write Enable Register - write 1's here to set the pins you want to enable.  
PIO_OER:  PIO (Parallel Input Output) Controller Output Enable Register - write 1's here to set pins as OUTPUT.  
After `0b` there are 32 bits (each port in this microcontroller is 32 bits). The first bit on the right corresponds to port pin 0, increasing leftward.  
So by using `REG_PIOD_OER  = 0b00000000000000000000000001000101;` we set PD0, PD2, and PD6 as output pins.

Then, within `loop()`, just use `REG_PIOX_ODSR`, instead of `digitalWrite`
```
REG_PIOD_ODSR = 0b00000000000000000000000001000101; // sets PD0, PD2, PD6 to HIGH
...
REG_PIOD_ODSR = 0b00000000000000000000000000000000; // sets PD0, PD2, PD6 to LOW
```

`PIO_ODSR` allows synchronous writing of the lines in the port (for more info, see 31.5.5 of [SAM3X datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-11057-32-bit-Cortex-M3-Microcontroller-SAM3X-SAM3A_Datasheet.pdf)).


### Useful links about Port manipulation on Arduino Due:

- <https://arduino.stackexchange.com/questions/9117/sam3x8e-arduino-due-pin-io-registers> (useful)
- <https://forum.arduino.cc/index.php?topic=537165.0>
- <https://forum.arduino.cc/index.php?topic=129868.30>
- <https://forum.arduino.cc/index.php?topic=260731.0>
- <https://forum.arduino.cc/index.php?topic=394428.0>  (maybe this useful)
- <http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-11057-32-bit-Cortex-M3-Microcontroller-SAM3X-SAM3A_Datasheet.pdf> (section 31)


### Pinout mapping schemes Arduino Due:

- <https://forum.arduino.cc/index.php/topic,132130.0.html>
- <https://content.arduino.cc/assets/Pinout-Due_latest.pdf>
- <https://www.arduino.cc/en/Hacking/PinMappingSAM3X>
