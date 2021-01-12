Tool for converting a wired keyboard to wireless with an RPi and Arduino and two nRF24L01 modules. The RPi connects to the keyboard and the Arduino acts as a receiver.

Requires HoodLoader2 to utilize the USB controller on the Arduino: https://github.com/NicoHood/HoodLoader2  
Requires tmrh20's nRF24 driver: https://github.com/nRF24/RF24  

Required libraries:  
PyUSB on RPi, tmrh20 nRF24 library on both, HoodLoader2 on Arduino's 16u2  

Installation:  

Wire nRF24l01 to RPi and Arduino, it uses SPI. There are wiring diagrams online  
Set up RPi to be a clean install of Raspbian.  
Update the key to a 16-byte value in keyboard_receiver_arduino.ino and keyboard_transmitter.py  
Flash keyboard_receiver_arduino to Arduino  
Flash keyboard_receiver_16u2 to 16u2  
Put keyboard_transmitter.py on the RPi  
  
This is designed and configured to work with a Ducky One 2 keyboard. It will almost certainly require changing endpoints around to work with other keyboards.  
  
In the future I'm going to replace the RPi with an STM32 that can act as a USB host for lower power consumption.  
  
Keyboard -> PC Packet format:  

xXXXXXXICCCCCCRH  
xX = Payload (Padded with random numbers if the report doesn't use all 7 bytes)  
I = USB Interface (1 or 2, if it's 2 the report number will be in x)  
C = 24-bit packet counter, resets with the iv at power-on. The receiver will need power-cycled to accept a new IV.  
R = Random data  
H = Checksum, all previous 15 bytes XOR'd together with an initial value of 0.  

This 16-byte block is encrypted with AES128 and sent with the IV from the keyboard to the receiver in 32-byte packets.  



PC -> Keyboard Packet Format  

L################################  

L = LED (Num lock/Caps lock/Scroll Lock) bitmask status  
\# = Zeroes  

These packets are not encrypted so don't transmit any sensitive information over Morse Code to your numlock LED.  
