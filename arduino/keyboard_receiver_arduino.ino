#include <SPI.h>
#include "printf.h"
#include "RF24.h"  # tmrh20 version in specific
#include <AESLib.h>  

char key[] = "<Put 16 random bytes here>";

RF24 radio( 9, 10 );

uint8_t address[][6] = { "1Node", "2Node" };
bool radioNumber = 1;

char payloadData[32] = { 0 } ;
bool ivSet = false;
aes_context ctx;

char LEDs[] = { 0 };

union _inPacketCounter
{
  char data[8];
  unsigned long long packetCounter;
} inPacketCounter;

unsigned long long packetCounter = 0;

void setup() {

  Serial.begin( 115200 );
  while ( !Serial ) {}

  if ( !radio.begin() ) {
    Serial.println( F( "radio hardware is not responding!!" ) );
    while (1) {} // hold in infinite loop
  }

  radioNumber = 1;

  radio.setPALevel( RF24_PA_MAX );
  radio.setPayloadSize( sizeof( payloadData ) );
  radio.openWritingPipe( address[radioNumber] );
  radio.openReadingPipe( 1, address[!radioNumber] );
  radio.setAutoAck( 1 );
  radio.enableDynamicAck();
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.startListening();
}

void loop() {
  uint8_t pipe;
  if ( radio.available( &pipe ) ) {
    uint8_t bytes = radio.getPayloadSize();
    char iv[16];
    char message[16];
    radio.read( &payloadData, bytes );
    memcpy( message, payloadData, 16 );
    memcpy( iv, payloadData + 16, 16 );
    if ( !ivSet )
    {
      ctx = aes128_cbc_dec_start( (const uint8_t*)key, iv );
    }
    
    aes128_cbc_dec_continue(ctx, message, 16);
    
    if ( !ivSet )
    {
      ivSet = true;
      return;
    }
    
    memcpy( inPacketCounter.data + 2, message + 9, 6 );
    int checksum = 0;
    for (int i=0;i<15;i++)
    {
      checksum ^= message[i];
    }
    if (checksum == message[15])
    {
      if ( inPacketCounter.packetCounter > packetCounter )
      {
        Serial.write( message, 16 );
        Serial.print( "NH45" );
        packetCounter = inPacketCounter.packetCounter;
      }
      else
      {
        //Serial.write( "Old packet counter received (Playback?)" );
      }
    }
    else
    {
      //Serial.write("Bad checksum!");
    }
  }
  if (Serial.available())
  {
    byte c = Serial.read();
    char output[32] = { 0 };
    output[0] = c;
    radio.stopListening();
    radio.write(output,32);
    radio.startListening();
  }
} // loop 
