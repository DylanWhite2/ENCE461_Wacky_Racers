/* File:   radio_tx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
   Descr: 
*/
#include "Nrf24.hpp"
#include "Pio.hpp"
#include "Pacer.hpp"
#include "stdio.h"
#include <math.h>

#include "Led.hpp"
#include "UsbSerial.hpp"
#include "Mpu9250.hpp"

const uint8_t X_INDEX = 0;
const uint8_t Y_INDEX = 1;
const uint8_t Z_INDEX = 2;
#define X_COEFF (1)
#define Y_COEFF (1)
#define Z_COEFF (1)
#define PI (3.14159265897323)
#define RESOLUTION (1000)
#define MAX_ANG (60)

int main (void)
{
    Wacky::UsbSerial usbserial;
    Wacky::Mpu9250 imu;
    Wacky::Led led2 (LED2_PIO, Wacky::ActiveLow);
    Wacky::Led led1 (LED1_PIO, Wacky::ActiveLow);
    Wacky::Nrf24 radio;
    Wacky::Nrf24::status status;
    Wacky::Pacer pacer(1);
    int16_t acceldata[3];
    char buffer[32];
    char pitch_str[32];
    char roll_str[32];
    double pitch;
    double roll;
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t drive_command;
    int16_t  turn_command;

    usbserial.begin ();
    led1=0;
    led2=0;
    
    status = radio.begin (42, 0xCCCECCCECC);
    
    while (! imu.begin (Wacky::Mpu9250::MPU_0)) {
        led2=1;
        usbserial.println ("Cannot detect IMU");
        }
    
    while (true) {
		pacer.wait();
        
		imu.readAccel (acceldata);
        x_accel = X_COEFF*acceldata[X_INDEX];
        y_accel = Y_COEFF*acceldata[Y_INDEX];
        z_accel = Z_COEFF*acceldata[Z_INDEX];
		pitch = (atan2(x_accel, z_accel)*180)/PI;
		roll  = (atan2(y_accel, z_accel)*180)/PI;
		
		if (fabs(pitch)<MAX_ANG) {
			drive_command = -floor((pitch*RESOLUTION)/MAX_ANG);
		} else if (pitch>0) {
			drive_command = -RESOLUTION;
		} else {
			drive_command = RESOLUTION;
		}
		
		if (fabs(roll)<MAX_ANG) {
			turn_command  = floor((roll*RESOLUTION)/MAX_ANG);
		} else if (roll>0) {
			turn_command = RESOLUTION;
		} else {
			turn_command = -RESOLUTION;
		}

        sprintf(buffer, "%d,%d", drive_command, turn_command);
        usbserial.println(buffer);
        
        
        if (! radio.write (buffer, sizeof (buffer))){
            led2.toggle ();
        }
        
        if (z_accel < 0) {
            led1=1;
        } else {
            led1=0;
        }
    }
}

