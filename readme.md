Tool for converting a wired keyboard to wireless with an RPi and Arduino and two nRF24L01 modules. The RPi connects to the keyboard and the Arduino acts as a receiver.

Requires HoodLoader2 to utilize the USB controller on the Arduino: https://github.com/NicoHood/HoodLoader2
Requires tmrh20's nRF24 driver: https://github.com/nRF24/RF24

Required libraries:
PyUSB on RPi, tmrh20 nRF24 library on both, HoodLoader2 on Arduino's 16u2

Installation:

Set up RPi to be a clean install of Raspbian.
Update the key to a 16-byte value in keyboard_receiver_arduino.ino and keyboard_transmitter.py
Flash keyboard_receiver_arduino to Arduino
Flash keyboard_receiver_16u2 to 16u2
Put keyboard_transmitter.py on the RPi

This is designed and configured to work with a Ducky One 2 keyboard. It will almost certainly require changing endpoints around to work with other keyboards.

In the future I'm going to replace the RPi with an STM32 that can act as a USB host for lower power consumption.
