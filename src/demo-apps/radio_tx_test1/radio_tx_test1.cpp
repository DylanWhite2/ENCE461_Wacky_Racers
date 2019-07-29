/* File:   radio_tx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
   Descr: 
*/
#include "Nrf24.hpp"
#include "Pio.hpp"
#include "Pacer.hpp"
#include "stdio.h"
#include "UsbSerial.hpp"

int main (void)
{
    Wacky::Pio led1 (LED1_PIO);
    Wacky::Pio led2 (LED2_PIO);
    Wacky::Pacer pacer(10);
    Wacky::Nrf24 radio;
    Wacky::Nrf24::status status;
    uint8_t count = 0;
    
    Wacky::UsbSerial usbserial; 

    usbserial.begin ();  

    led1.setMode (PIO_OUTPUT_HIGH);
    led2.setMode (PIO_OUTPUT_LOW);    
    
    status = radio.begin (42, 0xCCCECCCECC);
    if (status != Wacky::Nrf24::OK)
    {
        while (1)
        {
            pacer.wait ();

            led1.toggle ();            
            led2.toggle ();
        }
    }
        
    while (1)
    {
        char buffer[32];
        
        pacer.wait ();

        led2.toggle ();

        sprintf (buffer, "Hello world %d", count++);
        
        usbserial.println (status); 
        
        if (! radio.write (buffer, sizeof (buffer)))
            led1 = 0;
    }
}
