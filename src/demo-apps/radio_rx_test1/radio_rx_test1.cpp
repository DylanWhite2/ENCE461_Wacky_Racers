/* File:   radio_rx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
   Descr: 
*/
#include "Nrf24.hpp"
#include "UsbSerial.hpp"
#include "Pio.hpp"


int main (void)
{ 
    Wacky::Pio led1 (LED1_PIO);
    Wacky::Pio led2 (LED2_PIO);    
    Wacky::Nrf24 radio;
    Wacky::UsbSerial usbserial; 

    usbserial.begin ();        

    led1.setMode (PIO_OUTPUT_HIGH);
    led2.setMode (PIO_OUTPUT_LOW);    

    radio.begin (20, 0xCCCECCCECC);    

    radio.listen ();
    
    while (1)
    {
        char buffer[32];
        
        if (radio.read (buffer, sizeof (buffer)))
        {
            usbserial.println (buffer);            
            led1 = 0;
        }
    }
}
