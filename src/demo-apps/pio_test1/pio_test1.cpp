/* File:   pio_test1.c
   Author: M. P. Hayes, UCECE
   Date:   3 May 2018
   Descr: 
*/
#include "Pio.hpp"
#include "Pacer.hpp"
#include "mcu.h"

#define PACER_RATE 10

int main (void)
{
    Wacky::Pio pio1 (PB4_PIO);
    Wacky::Pio pio2 (PB5_PIO);                
    Wacky::Pacer pacer (PACER_RATE);    

    /* This is required if PB4 or PB5 is used since they default
       to JTAG pins.   */
    mcu_jtag_disable ();

    pio1.setMode (PIO_OUTPUT_HIGH);
    pio2.setMode (PIO_OUTPUT_LOW);
    
    while (1)
    {
        pacer.wait ();

        pio1.toggle ();
        pio2.toggle ();
    }
}
