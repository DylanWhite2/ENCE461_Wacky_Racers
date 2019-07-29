/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   12 February 2018
    @brief 
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a racer!  */

/* System clocks  */
#define F_XTAL 12.00e6
#define MCU_PLL_MUL 16
#define MCU_PLL_DIV 1

#define MCU_USB_DIV 2
/* 192 MHz  */
#define F_PLL (F_XTAL / MCU_PLL_DIV * MCU_PLL_MUL)
/* 96 MHz  */
#define F_CPU (F_PLL / 2)

/* TWI  */
#define TWI_TIMEOUT_US_DEFAULT 10000

/* USB  */
#define USB_VBUS_PIO PA10_PIO
#define USB_CURRENT_MA 500

/* LEDs  */
#define LED1_PIO PA0_PIO
#define LED2_PIO PA1_PIO
#define LED3_PIO PA3_PIO

/* General  */
#define APPENDAGE_EN PA26_PIO //WACKY_EN
#define GAMEBOARD_EN PA27_PIO
#define RF_EN PA4_PIO
#define BATT_MON AD0_PIO

/* Button  */
#define BUTTON_PIO PA2_PIO

/* H-bridges   */
#define MOTOR_AIN1_PWM_PIO PA11_PIO
#define MOTOR_AIN2_PWM_PIO PA24_PIO

#define MOTOR_BIN1_PWM_PIO PA25_PIO
#define MOTOR_BIN2_PWM_PIO PA7_PIO

#define MOTOR_SLEEP_PIO PA31_PIO

/* Radio  */
#define RADIO_CS_PIO PA29_PIO
#define RADIO_CE_PIO PA30_PIO
#define RADIO_IRQ_PIO PA28_PIO


#endif /* TARGET_H  */
