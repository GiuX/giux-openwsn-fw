/**
\brief This program shows the use of the "radio" bsp module.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
*/

#include "stdint.h"
#include "board.h"
#include "radio.h"
#include "leds.h"

#define LENGTH_PACKET 100
#define CHANNEL       26

/**
\brief The program starts executing here.
*/
int main(void)
{  
   uint8_t packet[LENGTH_PACKET+1];
   uint8_t i;
   
   // initialize
   board_init();
   
   // prepare packet
   packet[1] = LENGTH_PACKET;
   for (i=0;i<LENGTH_PACKET;i++) {
      packet[i+1] = i;
   }
   
   // send packet
   radio_setFrequency(CHANNEL);
   radio_rfOn();
   radio_loadPacket(&packet[1],LENGTH_PACKET+1);
   radio_txEnable();
   radio_txNow();
   radio_rfOff();
   led_radio_toggle();
   
   // go back to sleep
   board_sleep();
}