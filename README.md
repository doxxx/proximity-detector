# Garage Door Proximity Detector

## STM32 Blue Pill to RFM69HCW hookup

* 3.3V -> 13
* GND -> 1, 6 (Reset)
* PA3 -> 14 (DIO0)
* PA4 -> 5 (NSS)
* PA5 -> 4 (SCK)
* PA6 -> 2 (MISO)
* PA7 -> 3 (MOSI)

## STM32 Blue Pill to GP2Y0A51SK0F IR Detector

* 5V -> Red
* GND -> Black
* PA0 -> White

## Breadbooard layout

* STM32 Blue Pill VB in 1d
* RFM69HCW Pin 1 in 22d

## Black Magic Probe hookup

Using BMP on STM32 Blue Pill to target on STM32 Blue Pill:

* SWD -> SWD
* Serial
  * PB6 -> A10
  * PB7 -> A9
* Reset
  * PB4 -> RESET
