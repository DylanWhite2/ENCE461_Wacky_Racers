/* File:   pwm_test1.c
   Author: M. P. Hayes, UCECE
   Date:   23 Feb 2018
   Descr:  This demonstrates how to drive an A5995 dual H-bridge with PWM.
   * 5/4/19 UPDATE: REMOVED MODE/PHASE from A5995 code and added 4 channels of PWM
*/
#include "UsbSerial.hpp"
#include "Pwm.hpp"
#include "stdio.h"
#include "Pio.hpp"

int main (void)
{
    Wacky::UsbSerial usbserial;
    Wacky::Pwm pwmA1;
    Wacky::Pwm pwmA2;
    Wacky::Pwm pwmB1;
    Wacky::Pwm pwmB2;
    Wacky::Pio motor_sleep_pio (MOTOR_SLEEP_PIO);

    /* Low to sleep.  */
    motor_sleep_pio.setMode (PIO_OUTPUT_HIGH);
         
    pwmA1.begin (MOTOR_AIN1_PWM_PIO, 1000, 1000);
    pwmA2.begin (MOTOR_AIN2_PWM_PIO, 1000, 300);
    
    pwmB1.begin (MOTOR_BIN1_PWM_PIO, 1000, 1000);
    pwmB2.begin (MOTOR_BIN2_PWM_PIO, 1000, 300);

    usbserial.begin ();
    usbserial.print ("> ");
        
    while (1)
    {
        char buffer[80];
        int speed;

        if (!usbserial.readln (buffer, sizeof (buffer)))
            continue;
            
        if (sscanf (buffer, "%d", &speed) != 1)
            continue;

        //~ pwmA1.setDuty (speed * 10);
        pwmA2.setDuty (1000 - (speed * 10));
        //~ pwmB1.setDuty (speed * 10);
        pwmB2.setDuty (1000 - (speed * 10));
        usbserial.print (">>> ");        
    }
}
