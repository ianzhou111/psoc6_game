/**
 * @file hw01.h
 * @author Yining Wang (ywang2739@wisc.edu)
 * @author Ian Zhou (izhou3@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __HW01_H__
#define __HW01_H__

#include "hw01_draw.h"
#include "hw01_timer.h"

//enum type which builds the state machine
typedef enum
{
    SET_TIME_MODE,//default mode, set the current time
    RUN_MODE, // start to run the clock
    SET_ALARM_MODE, // set alarm
} hw01_state;

/**
 * @brief 
 * Initializes the PSoC6 Peripherals used for HW01 
 */
void hw01_peripheral_init(void);

/**
 * @brief 
 * Implements the main application for HW01 
 */
void hw01_main_app(void);


#endif