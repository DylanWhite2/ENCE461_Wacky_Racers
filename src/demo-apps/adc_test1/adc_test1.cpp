/* File:   adc_test1.cpp
   Author: M. P. Hayes, UCECE
   Date:   25 Feb 2018
   Descr:  This triggers ADC conversions on two channels in software
            as each sample is read.
*/

#include "Led.hpp"
#include "Adc.hpp"
#include "Pacer.hpp"
#include "UsbSerial.hpp"

/* Number of samples to read each time.  
   The first sample is from AD5 and the second is from AD6.  */
#define ADC_SAMPLES 1

int main (void)
{
    Wacky::Led led1 (LED1_PIO, Wacky::ActiveLow);
    Wacky::Led led2 (LED2_PIO, Wacky::ActiveLow);
    Wacky::Pacer pacer(10000);
    Wacky::UsbSerial usbserial;
    Wacky::Adc adc;
    uint16_t count = 0;
    
    led1 = 1;
    led2 = 0;

    usbserial.begin ();

    adc.begin (ADC_CHANNEL_0);
    
    while (1)
    {
		pacer.wait ();
		
        int16_t buffer[ADC_SAMPLES];
        int16_t bytes;
        count++;

      
        /* Read raw 12-bit unsigned data.  */
        
        
        if (count > 10000 / 1 / 2)
		{
			count = 0;
			bytes = adc.read (buffer, sizeof (buffer));
			usbserial.println (buffer[0]);
		}


    }
}
