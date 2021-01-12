#!/bin/python3
import sys
import argparse
import struct
from RF24 import RF24, RF24_PA_LOW, RF24_PA_MAX
from Crypto.Cipher import AES
import usb.core
import usb.util
import threading
import random
import time

packetCounter = 0

dev = usb.core.find( idVendor=0x04d9, idProduct=0x0296 )
print(dev)
interface = 0
endpoint = dev[0][(interface,0)][0]

interface2 = 2
endpoint2 = dev[0][(interface2,0)][0]

readWireless = True

if dev.is_kernel_driver_active( interface ) is True:
  dev.detach_kernel_driver( interface )
  usb.util.claim_interface( dev, interface )

if dev.is_kernel_driver_active( interface2 ) is True:
  dev.detach_kernel_driver( interface2 )
  usb.util.claim_interface( dev, interface2 )

key = b""  # Put 16 bytes of randomness that matches the arduino's random here.

PAYLOAD_SIZE = 16
PAYLOAD_INTERFACE_COUNTER_POS = 8

cipher = AES.new( key, AES.MODE_CBC )
radio = RF24( 25, 0 )

def sendData( data, interface ):
    global packetCounter;
    packetCounter = packetCounter + 1

    if len( data ) > PAYLOAD_SIZE:
        print( "Too much data to send {}".format( data ) )
        return

    #print( "Sending {}".format( data ) )

    buffer = bytearray( data )
    while len(buffer) < PAYLOAD_SIZE:
        if len(buffer) == PAYLOAD_INTERFACE_COUNTER_POS:
            buffer.append( interface )
            buffer.extend( packetCounter.to_bytes( 6, byteorder='little' ) )
        if len(buffer) == 15:
            value = 0
            for byte in buffer:
                value = value ^ byte
            #print("Checksum: {}".format(value))
            buffer.append( value )
        else:
            buffer.append( round( random.random() * 255 ) )

    encryptedMessage = bytearray( cipher.encrypt( buffer ) )
    encryptedMessage.extend( cipher.iv )
    readWireless = False
    radio.stopListening()
    result = radio.write( encryptedMessage )
    radio.startListening()
    readWireless = True

    failCount = 0
    while not result and failCount < 10:
        print( "Transmission failed or timed out" )
        readWireless = False
        radio.stopListening()
        result = radio.write( encryptedMessage )
        radio.startListening()
        readWireless = True
        failCount = failCount + 1

def listen1():
    print( "Listening to Keyboard Endpoint" )
    while True:
        time.sleep(.05)
        try:
            data = dev.read( endpoint.bEndpointAddress, endpoint.wMaxPacketSize, -1 )
            sendData( data, 1 )
        except usb.core.USBError as e:
            data = None
            if e.args == ( 'Operation timed out', ):  #TODO error
                continue

def listen2():
    print( "Listening to ConsumerControl/Mouse Endpoint" )
    while True:
        time.sleep(.01)
        try:
            data = dev.read( endpoint2.bEndpointAddress, endpoint2.wMaxPacketSize, -1 )
            sendData( data, 2 )
        except usb.core.USBError as e:
            data = None
            if e.args == ( 'Operation timed out', ):
                continue

def listen3():
    while True:
        time.sleep(.1)
        if readWireless:
            has_payload, pipe_number = radio.available_pipe()
            if has_payload:
                buffer = radio.read(32)
                result = [ buffer[0] ]
                dev.ctrl_transfer(0x21, 0x9, 0x0200, 0, result)



def master():
    print( "Starting USB threads" )
    radio.startListening()  # put radio in RX mode
    thread1 = threading.Thread( target=listen1 )
    thread2 = threading.Thread( target=listen2 )
    thread3 = threading.Thread( target=listen3 )
    thread1.start()
    thread2.start()
    thread3.start()


if __name__ == "__main__":

    if not radio.begin():
        raise RuntimeError( "radio hardware is not responding" )
    address = [ b"1Node", b"2Node" ]
    print( "Setting up radio" )
    radio_number = 0
    radio.setPALevel( RF24_PA_MAX )  # RF24_PA_MAX is default
    radio.openWritingPipe( address[radio_number] )  # always uses pipe 0
    radio.openReadingPipe( 1, address[not radio_number] )  # using pipe 1
    radio.payloadSize = PAYLOAD_SIZE * 2
    radio.setRetries( 1, 15 )
    radio.setAutoAck( 1 )
    radio.enableAckPayload()
    radio.enableDynamicAck()
    radio.enableDynamicPayloads()
    print( "Radio configured and ready" )

    try:
        master()
    except KeyboardInterrupt:
        print( "Keyboard Interrupt detected. Exiting..." )
        radio.powerDown()
        usb.util.release_interface( dev, interface )
        dev.attach_kernel_driver( interface )
        usb.util.release_interface( dev, interface2 )
        dev.attach_kernel_driver( interface2 )
        sys.exit()
