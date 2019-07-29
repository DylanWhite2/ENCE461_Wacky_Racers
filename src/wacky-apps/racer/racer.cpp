//if shit stops working, make clean.

/** @file   racer.cpp
    @author M. P. Hayes, UCECE
    @date   12 February 2018
*/
#include "Pacer.hpp"
#include "Adc.hpp"
#include "Led.hpp"
#include "Pio.hpp"
#include "Nrf24.hpp"
#include "Mpu9250.hpp"
#include "UsbSerial.hpp"
#include "UsartSerial.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mcu.h"
#include "Pwm.hpp"
#include "patterns.h"

#define PACER_RATE 10000
#define LED_FLASH 1
#define LINE_SIZE 80
#define BATTERY_LIM 1580
#define DEBUG false
#define APP_CONTROL false //solid blue LED indicates APP_CONTROL, flashing indicates BOARD.

static uint8_t r,g,b;
static int count = 0;
static int count2 = 0;
static int count3 = 0;
static uint16_t battV;
static int sleepFlag = 0;
static int ledFlag = 1;
static int patternFlag = 0;
static int testFlag = 1;

//~ mcu_sleep_mode_t mode = MCU_SLEEP_MODE_BACKUP;
//~ mcu_sleep_cfg_t sleep = {mode, PA2_PIO, true};
//~ SUPC->SUPC_WUMR = 0x00000000 | (4 << 12);

void setSpeed(int speed, int dir, Wacky::Pwm pwmA1, Wacky::Pwm pwmA2, Wacky::Pwm pwmB1, Wacky::Pwm pwmB2, Wacky::UsbSerial usbserial)
{
	if (patternFlag)
	{
		return;
	}
	if (speed == 0)
	{
		pwmA1.setDuty(1000); //forward
		pwmB1.setDuty(1000); //forward
		
		if (dir < 0)
		{
			dir *= -1;
			pwmA2.setDuty(1000 - speed); 				//right	
			pwmB2.setDuty(1000 - dir); 					//left
		} 
		else if (dir > 0)
		{
			pwmA2.setDuty(1000 - dir); 					//right	
			pwmB2.setDuty(1000 - speed); 				//left	
		}	
		else if (dir == 0)
		{
			pwmA2.setDuty(1000 - speed); 				//right	
			pwmB2.setDuty(1000 - speed); 				//left	
		}
	}
	
	else if (speed > 0)
	{
		pwmA1.setDuty(1000); //forward
		pwmB1.setDuty(1000); //forward
		
		if (dir < 0) //turning right
			{
				dir *= -1;
				pwmA2.setDuty(1000 - (speed - (dir * 0.25))); 		//right	
				pwmB2.setDuty(1000 - speed); 				//left
			} 
		else if (dir > 0)
			{
				pwmA2.setDuty(1000 - speed); 				//right	
				pwmB2.setDuty(1000 - (speed - (dir * 0.25))); 		//left;
			}
		else if (dir == 0)
		{
			pwmA2.setDuty(1000 - speed); 				//right	
			pwmB2.setDuty(1000 - speed); 				//left		
		}	
	}
	
	else if (speed < 0)
	{
		speed *= -1;
		pwmA2.setDuty(1000); //reverse
		pwmB2.setDuty(1000); //reverse
		
		if (dir < 0) //turning right
			{
				dir *= -1;
				pwmA1.setDuty(1000 - (speed - (dir * 0.25))); 		//right	
				pwmB1.setDuty(1000 - speed); 				//left
			} 
		else if (dir > 0)
			{
				pwmA1.setDuty(1000 - speed); 				//right	
				pwmB1.setDuty(1000 - (speed - (dir * 0.25))); 		//left;
			}
		else if (dir == 0)
		{
			pwmA1.setDuty(1000 - speed); 				//right	
			pwmB1.setDuty(1000 - speed); 				//left		
		}	
		
	}
}

static void
killMotors(Wacky::Pio motorPower)
{
	motorPower.setMode (PIO_OUTPUT_LOW);		//TURN OFF MOTORS
}

static void
restartMotors(Wacky::Pio motorPower)
{
	motorPower.setMode (PIO_OUTPUT_HIGH);		//TURN ON MOTORS
}

static void 
idleMotors(Wacky::Pwm pwmA1, 
			 Wacky::Pwm pwmA2, 
			 Wacky::Pwm pwmB1, 
			 Wacky::Pwm pwmB2)
{
	pwmA1.begin (MOTOR_AIN1_PWM_PIO, 1000, 1000);
    pwmA2.begin (MOTOR_AIN2_PWM_PIO, 1000, 700);
    pwmB1.begin (MOTOR_BIN1_PWM_PIO, 1000, 1000);
    pwmB2.begin (MOTOR_BIN2_PWM_PIO, 1000, 700);
}


static void
sleep(Wacky::Pio motorPower,
				Wacky::Pio gameboardPower, Wacky::Pio radioPower, Wacky::Led blueLED)
{
	motorPower.setMode (PIO_OUTPUT_LOW);		//TURN OFF MOTORS
	gameboardPower.setMode (PIO_OUTPUT_LOW); 	//TURN OFF GAMEBOARD
	radioPower.setMode (PIO_OUTPUT_LOW);		//TURN OFF RADIO
	//mcu_sleep(&sleep);
	ledFlag = 0;
}

static void
wake(Wacky::Pio motorPower,
				Wacky::Pio gameboardPower, 
				Wacky::Pio radioPower, 
				Wacky::Pwm pwmA1, 
				Wacky::Pwm pwmA2, 
				Wacky::Pwm pwmB1, 
				Wacky::Pwm pwmB2,
				Wacky::Nrf24 radio,
				Wacky::Led blueLED)
{
	//mcu_sleep(&sleep);
	motorPower.setMode (PIO_OUTPUT_HIGH);		//TURN OFF MOTORS
	gameboardPower.setMode (PIO_OUTPUT_HIGH); 	//TURN OFF GAMEBOARD
	radioPower.setMode (PIO_OUTPUT_HIGH);		//TURN OFF RADIO
	ledFlag = 1;
	
	idleMotors(pwmA1, pwmA2, pwmB1, pwmB2);
	radio.begin (20, 0xCCCECCCECC); 
    radio.listen (); 
}

static void
checkBattery(bool batteryGood, Wacky::Pio motorPower,
				Wacky::Pio gameboardPower, Wacky::Pio radioPower)
{
	if (!batteryGood)
	{
		motorPower.setMode (PIO_OUTPUT_LOW);		//TURN OFF MOTORS
		gameboardPower.setMode (PIO_OUTPUT_LOW); 	//TURN OFF GAMEBOARD
		radioPower.setMode (PIO_OUTPUT_LOW);		//TURN OFF RADIO
	} 
}

static void 
gameboard_task (Wacky::UsartSerial &serial,
                            Wacky::UsbSerial &usbserial,
                            Wacky::Pio motorPower,
                            Wacky::Pio gameboardPower, 
                            Wacky::Pio radioPower,
                            Wacky::Pwm pwmA1,
                            Wacky::Pwm pwmA2,
                            Wacky::Pwm pwmB1,
                            Wacky::Pwm pwmB2,
                            Wacky::Led blueLED)
{
    char buffer[LINE_SIZE];
    char output[LINE_SIZE];
    const char *response = "";    

    if (!serial.readln (buffer, sizeof (buffer)))
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
             
     // How much current (mA) we can supply over the 5 V rail.
     // This should depend on the source; the battery can supply
     // more than USB.
   
    case 'U':
        response = "500";
        break;
        
    // Mode (test or comp)
    case 'M':
        response = "comp";
        break;        
        
        // Group number
    case 'N':
        response = "20";
        break;
        
        // Board type (hat or racer)
    case 'B':
        response = "racer";
        radioPower.setMode (PIO_OUTPUT_HIGH);
        break;
        
    case 'K':
		testFlag = 0;
		killMotors(motorPower);
		response = "";
		break;
        
    case 'R':
		testFlag = 1;
		restartMotors(motorPower);
		response = "";
		break;
		
	case 'L':
		patternFlag = 1;
		killMotors(motorPower);
		char temp;
		int pattern;
		sscanf (buffer, "%c%d", &temp, &pattern);
		if (pattern == 0) {
			
			if (count2 % 3 == 0)
			{
				response = winPatterns3[count];
				++count;
			}
			else {
				response = winPatterns[count];
				++count;
			}
			if (count == 29)
			{
				count = 0;
				count2++;
			}
		}
		else if (pattern == 1) {
			if (count2 % 3 == 0)
			{
				response = winPatterns3[count];
				++count;
			}
			else {
				response = winPatterns[count];
				++count;
			}
			if (count == 29)
			{
				count = 0;
				count2++;
			}
		}
		else if (pattern == 2) {
			if (count2 % 3 == 0)
			{
				response = losePatterns2[count];
				++count;
			}
			else {
				response = losePatterns[count];
				++count;
			}
			if (count == 29)
			{
				count = 0;
				count2++;
			}
		}
		sscanf (buffer, "%c%d", &temp, &pattern);
		break;

		
    // Group name
    case 'G':
        response = "Zoom";
        break;
        
        // Student names (comma separated)
    case 'S':
        response = "Dylan White, Callum Davidson";
        break;
        
    case 'V':
		patternFlag = 0;
		
		if (testFlag)
		{
			restartMotors(motorPower);
		}
		
		char voltBuff[80];
		sprintf(voltBuff, "%d", battV);
		response = voltBuff;
		break;
		
	case 'I':
		response = "0";
		break;
	
	case 'Q': 
		response = "0";
		break;
		
	case 'T':
		response = "";
		char temp2;
		int appSpeed;
		int appDir;
		sscanf(buffer, "%c%d,%d", &temp2, &appSpeed, &appDir);
		setSpeed(appSpeed * 10, appDir * 10, pwmA1, pwmA2, pwmB1, pwmB2, usbserial);
		break;	
    
    case 'Z':
		response = "";
		sleep(motorPower, gameboardPower, radioPower, blueLED);
		break;
		
	}
    
    serial.println (response);
    
    if (DEBUG)
    {
        usbserial.print (">");
        usbserial.println (response);
    }
}

int
main (void)
{
	int flashTicks = 0;
	int adcTicks = 0;
	static bool batteryGood = true;
	int speed = 0;
    int dir = 0;
    char radioBuffer[80];
    int batteryCount = 0;
    int sleepCount = 0;
    int buttonTimer = 0;
	
	int16_t adcBuffer[1];
	    
    Wacky::Pacer pacer (PACER_RATE); 
    Wacky::UsbSerial usbserial;
    Wacky::UsartSerial serial;
    Wacky::Adc adc;
    Wacky::Nrf24 radio;
    Wacky::Pwm pwmA1;
    Wacky::Pwm pwmA2;
    Wacky::Pwm pwmB1;
    Wacky::Pwm pwmB2;
    Wacky::Pio motorPower (MOTOR_SLEEP_PIO);
    Wacky::Pio gameboardPower (GAMEBOARD_EN); 
    Wacky::Pio radioPower (RF_EN);  
    Wacky::Pio sleepButton(BUTTON_PIO);  
    Wacky::Led blueLED (LED1_PIO, Wacky::ActiveLow);
    Wacky::Led redLED (LED2_PIO, Wacky::ActiveLow);
    
    sleepButton.setMode (PIO_PULLUP);
    motorPower.setMode (PIO_OUTPUT_HIGH);
    gameboardPower.setMode (PIO_OUTPUT_HIGH);
    radioPower.setMode (PIO_OUTPUT_HIGH);
     
	//~ pwmA1.begin (MOTOR_AIN1_PWM_PIO, 1000, 1000); //never ever ever fucking change this dylan.
    //~ pwmA2.begin (MOTOR_AIN2_PWM_PIO, 1000, 1000);
    //~ pwmB1.begin (MOTOR_BIN1_PWM_PIO, 1000, 1000);
    //~ pwmB2.begin (MOTOR_BIN2_PWM_PIO, 1000, 1000);
    
	idleMotors(pwmA1, pwmA2, pwmB1, pwmB2);	//test bump switch with this. 

    serial.begin (Wacky::UsartSerial::USART_0, 250000);
    usbserial.begin ();
    usbserial.setEcho();
    
    adc.begin (ADC_CHANNEL_0);
    
    radio.begin (20, 0xCCCECCCECC); 
    radio.listen ();  
    
    // Disable reset pin as a work-around for a bug with the game board.
    mcu_reset_disable ();

    // If you are using PB4 or PB5 uncomment the next line.
    // mcu_jtag_disable ();    

    mcu_watchdog_enable ();
           
    while (1)
    {
        pacer.wait ();
        mcu_watchdog_reset ();
        gameboard_task (serial, usbserial, motorPower, gameboardPower, radioPower, pwmA1, pwmA2, pwmB1, pwmB2, blueLED);
        
		flashTicks++;
		adcTicks++;
		
		if (!sleepButton.read())
		{
			if (APP_CONTROL)
			{
				buttonTimer = PACER_RATE;
			} else
			{
				buttonTimer = 200; //200
			}
			
			sleepCount++;
			if (sleepCount > buttonTimer && !sleepFlag) //to work in board control = 200, to work in app control = PACER_RATE
			{
				sleep(motorPower, gameboardPower, radioPower, blueLED);
				sleepFlag = 1 - sleepFlag;
				sleepCount = 0;				
			} 
			else if (sleepCount > buttonTimer && sleepFlag)
			{
				wake(motorPower, gameboardPower, radioPower, pwmA1, pwmA2, pwmB1, pwmB2, radio, blueLED);
				sleepFlag = 1 - sleepFlag;
				sleepCount = 0;
			}
		}
		
		if (flashTicks > PACER_RATE / 200) //200
		{
			flashTicks = 0;
			if (ledFlag)
			{
				blueLED.toggle();
			} else {
				blueLED = 0;
			}
		}
		
		if (adcTicks > PACER_RATE / 100) //100
		{
			adcTicks = 0;
			adc.read (adcBuffer, sizeof (adcBuffer));
			battV = ((adcBuffer[0] * 1000) / 300); //shitty estimation cause ADC isn't working as expected. PoS
			if (adcBuffer[0] < BATTERY_LIM)
			{
				batteryCount++;
				
				if (batteryCount > 5) //lets make sure the battery is proper fucked before we actually kill it all.
				{
					batteryGood = false;
					redLED = 1;
				}
			} else {
				redLED = 0;
				batteryCount = 0;
			}
		}
          
        radio.read (radioBuffer, sizeof (radioBuffer));
        sscanf (radioBuffer, "%d,%d", &speed, &dir);
        //usbserial.println(radioBuffer);
        if (!APP_CONTROL) 
        {
			setSpeed(speed, dir, pwmA1, pwmA2, pwmB1, pwmB2, usbserial);
		}
		
		checkBattery(batteryGood, motorPower, gameboardPower, radioPower);
    }
    
    return 0;
}
