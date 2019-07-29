/* File:   gameboard_test1.c
   Author: M. P. Hayes, UCECE
   Date:   9 May 2018
   Descr:  Demonstration for interfacing with a gameboard.

   Notes:

   1. Change the group number, board number etc.  This is used for the
   WiFi hotspot name.

   2. The gameboard interfaces using a UART serial connection.  This can
   be driven by either the SAM4S UART or USART peripherals.  There are
   two of each:

   USART0: 
     RXD0 (PA5, pin 35)
     TXD0 (PA6, pin 34)

   USART1:
     RXD1 (PA21, pin 11)
     TXD1 (PA22, pin 14)

   UART0: 
     URXD0 (PA9, pin 30)
     UTXD0 (PA10, pin 29)

   UART1:
     URXD1 (PB2, pin 5)
     UTXD1 (PB3, pin 6)

   If you use UART0 or UART1, replace UsartSerial with UartSerial.

   The pin labelled RX on the gameboard connector is an output from
   the gameboard and must be connected to one of the RXD pins.

   The pin labelled TX on the gameboard connector is an input to
   the gameboard and must be connected to one of the TXD pins.

   3. If this program returns an invalid response (or no response say
   due to hardware fault), the gameboard runs in demo mode and will
   repeatedly send an error string over the UART interface.  The first
   LED will be white and the others will display a moving coloured
   pattern.
*/
#include "UsartSerial.hpp"
#include "UartSerial.hpp"
#include "UsbSerial.hpp"
#include "Pacer.hpp"
#include "mcu.h"
#include "Pio.hpp"
#include "stdio.h"
#include "stdlib.h"

#define PACER_RATE 1000
#define LINE_SIZE 80
#define DEBUG false

static int count = 0;
static int r, g ,b;


static void send_color(uint8_t r, uint8_t g, uint8_t b) {
	char buffer[10] = { 0 };
	sprintf(buffer, "<%x%x%x>", r, g, b);
}

static void gameboard_task (Wacky::UsartSerial &serial,
                            Wacky::UsbSerial &usbserial)
{
    char buffer[LINE_SIZE];
    char output[LINE_SIZE];
    const char *response = "";    

    if (! serial.readln (buffer, sizeof (buffer)))
        return;

    if (DEBUG)
    {
        usbserial.print ("<");
        usbserial.print (buffer);
    }
    
    switch (buffer[0])
    {
    default:
        break;
        
        // Board type (hat or racer)
    case 'B':
        response = "racer";
        break;
        
        // Group name
    case 'G':
        response = "Zoom";
        break;
        
        // Mode (test or comp)
    case 'M':
        response = "test";
        break;        
        
        // Group number
    case 'N':
        response = "20";
        break;
        
        // Student names (comma separated)
    case 'S':
        response = "Dylan White, Callum Davidson";
        break;
        
        // How much current (mA) we can supply over the 5 V rail.
        // This should depend on the source; the battery can supply
        // more than USB.
    case 'U':
        response = "300";
        break;
        
    case 'V':
		response = "1";
		break;
		
	case 'I':
		response = "5";
		break;
	
	case 'Q': 
		response = "50";
		break;
		
	case 'T':
		char appBuffer[80];
		char temp2;
		int appSpeed;
		int appDir;
		sscanf(buffer, "%c%d,%d", &temp2, &appSpeed, &appDir);
		sprintf(appBuffer, "%d,%d", appSpeed, appDir);
		usbserial.println(buffer);
		response = "";
		break;	
		
	case 'L':
		count++;
		char temp;
		int pattern;
		sscanf (buffer, "%c%d", &temp, &pattern);
		if (pattern == 0) {
			if (count % 5 == 0) {
				response = "<00B67A>";
			}
			else {
				sprintf(output, "<%02X%02X%02X>", 255, 0, 0);
				response = output;
			}

		}
		sscanf (buffer, "%c%d", &temp, &pattern);
		
		break;

    }
    
  
    serial.println (response);
    
    if (DEBUG)
    {
        usbserial.print (">");
        usbserial.println (response);
    }
}




int main (void)
{
    Wacky::UsartSerial serial;
    Wacky::UsbSerial usbserial;
    Wacky::Pacer pacer (PACER_RATE);   
    Wacky::Pio gameboardPower (GAMEBOARD_EN); 
    gameboardPower.setMode (PIO_OUTPUT_HIGH);

    // Disable the reset pin since when the game board powers up it
    // inadvertently toggles the PIO pin connected to the SAM4S reset
    // input.
    mcu_reset_disable ();
    
    usbserial.begin ();

    serial.begin (Wacky::UsartSerial::USART_0, 250000);

    while (1)
    {
        pacer.wait ();

        gameboard_task (serial, usbserial);
    }
}
