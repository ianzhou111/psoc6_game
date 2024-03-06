/**
 * @file ece353.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __MAIN_H__
#define __MAIN_H__

/* Include Infineon BSP Libraries */
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* Include Standard C Libraries*/
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>

/* Include ECE353 Drivers */
#include "drivers/console.h"
#include "drivers/eeprom.h"
#include "drivers/io-sw.h"
#include "drivers/io-leds.h"
#include "drivers/io-lcd.h"
#include "drivers/joystick.h"
#include "drivers/remote_uart.h"
#include "drivers/systick.h"
#include "drivers/timer.h"
#include "drivers/pwm-buzzer.h"

/* SPI Includes */
#include "drivers/spi.h"
#include "drivers/lsm6dsm_reg.h"
#include "drivers/imu.h"
#include "drivers/eeprom.h"

/* I2C Includes */
#include "drivers/i2c.h"
#include "drivers/LM75.h"
#include "drivers/io-expander.h"
#include "drivers/ft6x06.h"  /* ADD CODE */

/* FreeRTOS Includes */
#include <FreeRTOS.h>
#include <event_groups.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>



/* This macro is used to determine if we are building an executable for example code or ICE code
 * 
 * To choose ICE code, use the following line
 * #define ICE
 * 
 * To choose EXAMPLE code, use the following line
 * #undef ICE
*/
#undef ICE 
#undef HW01
#undef HW02

/* This macro identifies which ice/example file is being compiled into an executable*/
#define FILE_ID 16 

#if ! defined(ICE) 
#define EXAMPLE 
#else
#undef EXAMPLE 
#endif

extern char NAME[];
extern char project_DESCRIPTION[];
#endif