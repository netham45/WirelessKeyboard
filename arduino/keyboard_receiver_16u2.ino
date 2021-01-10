#include "HID.h"

static const uint8_t descriptor[] PROGMEM = {
  //  Keyboard
  0x05, 0x01,                      /* USAGE_PAGE (Generic Desktop)    47 */
  0x09, 0x06,                      /* USAGE (Keyboard) */
  0xa1, 0x01,                      /* COLLECTION (Application) */
  0x85, 0x02,                      /*   REPORT_ID */
  0x05, 0x07,                      /*   USAGE_PAGE (Keyboard) */

  /* Keyboard Modifiers (shift, alt, ...) */
  0x19, 0xe0,                      /*   USAGE_MINIMUM (Keyboard LeftControl) */
  0x29, 0xe7,                      /*   USAGE_MAXIMUM (Keyboard Right GUI) */
  0x15, 0x00,                      /*   LOGICAL_MINIMUM (0) */
  0x25, 0x01,                      /*   LOGICAL_MAXIMUM (1) */
  0x75, 0x01,                      /*   REPORT_SIZE (1) */
  0x95, 0x08,                      /*   REPORT_COUNT (8) */
  0x81, 0x02,                      /*   INPUT (Data,Var,Abs) */

  /* Reserved byte, used for consumer reports, only works with linux */
  0x05, 0x0C,                  /*   Usage Page (Consumer) */
  0x95, 0x01,                      /*   REPORT_COUNT (1) */
  0x75, 0x08,                      /*   REPORT_SIZE (8) */
  0x15, 0x00,                      /*   LOGICAL_MINIMUM (0) */
  0x26, 0xFF, 0x00,                /*   LOGICAL_MAXIMUM (255) */
  0x19, 0x00,                      /*   USAGE_MINIMUM (0) */
  0x29, 0xFF,                      /*   USAGE_MAXIMUM (255) */
  0x81, 0x00,                      /*   INPUT (Data,Ary,Abs) */

  /* 5 LEDs for num lock etc, 3 left for advanced, custom usage */
  0x05, 0x08,            /*   USAGE_PAGE (LEDs) */
  0x19, 0x01,            /*   USAGE_MINIMUM (Num Lock) */
  0x29, 0x08,            /*   USAGE_MAXIMUM (Kana + 3 custom)*/
  0x95, 0x08,            /*   REPORT_COUNT (8) */
  0x75, 0x01,            /*   REPORT_SIZE (1) */
  0x91, 0x02,            /*   OUTPUT (Data,Var,Abs) */

  /* 6 Keyboard keys */
  0x05, 0x07,                      /*   USAGE_PAGE (Keyboard) */
  0x95, 0x06,                      /*   REPORT_COUNT (6) */
  0x75, 0x08,                      /*   REPORT_SIZE (8) */
  0x15, 0x00,                      /*   LOGICAL_MINIMUM (0) */
  0x26, 0xE7, 0x00,                /*   LOGICAL_MAXIMUM (231) */
  0x19, 0x00,                      /*   USAGE_MINIMUM (Reserved (no event indicated)) */
  0x29, 0xE7,                      /*   USAGE_MAXIMUM (Keyboard Right GUI) */
  0x81, 0x00,                      /*   INPUT (Data,Ary,Abs) */

  /* End */
  0xc0,                            /* END_COLLECTION */
  0x05, 0x0C, //Consumer collection
  0x09, 0x01, //Usage
  0xA1, 0x01, //Collection
  0x85, 0x03, //Report ID
  0x15, 0x00, //Logical Minimum
  0x25, 0x01, //Logical Maximum
  0x95, 0x12, //Report count
  0x75, 0x01, //Report size
  0x0A, 0x83,
  0x01, 0x0A,
  0x8A, 0x01,
  0x0A, 0x92,
  0x01, 0x0A,
  0x94, 0x01,
  0x09, 0xCD,
  0x09, 0xB7,
  0x09, 0xB6,
  0x09, 0xB5,
  0x09, 0xE2,
  0x09, 0xEA,
  0x09, 0xE9,
  0x0A, 0x21,
  0x02, 0x0A,
  0x23, 0x02,
  0x0A, 0x24,
  0x02, 0x0A,
  0x25, 0x02,
  0x0A, 0x26,
  0x02, 0x0A,
  0x27, 0x02,
  0x0A, 0x2A,
  0x02, 0x81,
  0x02, 0x95,
  0x01, 0x75,
  0x0E, 0x81,
  0x01, 0xC0,
  0x05, 0x01,
  0x09, 0x02,
  0xA1, 0x01, 
  0x09, 0x01,
  0xA1, 0x00,
  0x85, 0x04,
  0x05, 0x09,
  0x19, 0x01,
  0x29, 0x08,
  0x15, 0x00,
  0x25, 0x01,
  0x75, 0x01,
  0x95, 0x08,
  0x81, 0x02,
  0x05, 0x01,
  0x09, 0x30,
  0x09, 0x31,
  0x16, 0x01,
  0x80, 0x26,
  0xFF, 0x7F,
  0x75, 0x10,
  0x95, 0x02,
  0x81, 0x06,
  0x09, 0x38,
  0x15, 0x81,
  0x25, 0x7F, 
  0x75, 0x08,
  0x95, 0x01,
  0x81, 0x06,
  0x05, 0x0C,
  0x0A, 0x38,
  0x02, 0x95,
  0x01, 0x81,
  0x06, 0xC0,
  0xC0, 0x05,
  0x01, 0x09,
  0x06, 0xA1,
  0x01, 0x85,
  0x05, 0x05,
  0x07, 0x95,
  0x01, 0x75,
  0x08, 0x81,
  0x03, 0x95,
  0xE8, 0x75,
  0x01, 0x15,
  0x00, 0x25,
  0x01, 0x05, 
  0x07, 0x19,
  0x00, 0x29, 
  0xE7, 0x81,
  0x00, 0xC0
};

bool firstEntry = true;

const char magic[] = {'N', 'H', '4', '5'};

void setup() {
  static HIDSubDescriptor node(descriptor, sizeof(descriptor));
  HID().AppendDescriptor(&node);
  Serial.begin(115200);
  Serial1.begin(115200);
}
unsigned char serialBuffer[32];
int serialBufferHead = 0;
bool discardNext = false;
bool foundFlag = false;
byte LEDs = 0;
unsigned int tickCounter = 0;

void loop() {
  while ( Serial1.available() )
  {
    char c = Serial1.read();

    if ( serialBufferHead == sizeof( serialBuffer ) )
    {
      serialBufferHead = 0;
      discardNext = true;
    }

    serialBuffer[ serialBufferHead ] = c;
    serialBufferHead += 1;

    for ( int i = 0; i < serialBufferHead - 3; i++ )
    {
      if ( memcmp( serialBuffer + i, magic, sizeof( magic ) ) == 0 )
      {
        foundFlag = true;
        char* message = serialBuffer + ( i - 16 );
        int interface = message[8];

        if ( interface == 1 )
        {
          HID().SendReport( 2 , message, 8 );
        }
        else if ( interface == 2 )
        {
          int report = message[0] + 1;
          int length = report == 3 ? 4 : 7; //If we get a message on endpoint 2 it's length is 4 else it's on endpoint 3 and 7
          HID().SendReport( report , message + 1, length );
        }
        else
        {
          Serial.println( "Unknown interface ID received" );
        }
      }
    }
    if ( foundFlag )  //If data found reset the write head
    {
      foundFlag = false;
      serialBufferHead = 0;
    }
  }
  if (tickCounter++ == 50000)
  {
    LEDs = HID().LEDs;
    Serial1.write( LEDs );
    tickCounter = 0;
  }
}
