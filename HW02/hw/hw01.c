/**
 * @file hw01.c
 * @author Yining Wang (ywang2739@wisc.edu)
 * @author Ian Zhou (izhou3@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hw01.h"
#include "main.h"

/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
#define TICKS_MS_010 10000000

// initialize curr state for the state machine as set time
hw01_state curr = SET_TIME_MODE;

// initialize 5 interrupt to handle each button, time update and blink
extern hw01_timer_alerts_t sw1_state;
extern hw01_timer_alerts_t sw2_state;
extern hw01_timer_alerts_t sw3_state;
extern hw01_timer_alerts_t blink_state;
extern hw01_timer_alerts_t update_state;
/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void hw01_peripheral_init(void)
{
    /* Initialize the pushbuttons */
    push_buttons_init();

    /* Initalize the LEDs */
    leds_init();

    /* Initialize the LCD */
    ece353_enable_lcd();
    /* Initialize the buzzer */
    pwm_buzzer_init();

    /* Intitialize a timer to generate an interrupt every 100mS*/
    hw01_timer_init(TICKS_MS_010); /* MODIFY with correct Ticks Count for 100mS*/
}

/**
 * @brief
 * Implements the main application for HW01
 */
void hw01_main_app(void)
{
    static bool alarm_disable = true;   // true when alarm is disabled, false otherwise
    static bool alarm = false;          // true when alarm is on, false otherwise
    static int alarm_set_timer_min = 0; // alarm's set_timer_min
    static int alarm_set_timer_sec = 0; // alarm's set_timer_sec
    static int set_timer_min = 0;       // timer's set_timer_min
    static int set_timer_sec = 0;       // timer's set_timer_sec
    static bool set_set_timer_sec = false;
    static bool set_alarm_set_timer_sec = false;
    while (1)
    {
        switch (curr)
        {
        case SET_TIME_MODE:
        {
            // Mode set time, where you set the current timer
            if ((sw1_state != 0) && (set_set_timer_sec == false))
            {
                // increment timer min when sw1 pushed
                sw1_state = HW01_ALERT_NONE;
                if (set_timer_min == 2)
                {
                    set_timer_min = 0;
                }
                else
                {
                    set_timer_min++;
                }
            }
            else if ((sw2_state != HW01_ALERT_NONE) && (set_set_timer_sec == false))
            {
                // decrement timer min when sw2 pushed
                sw2_state = HW01_ALERT_NONE;
                if (set_timer_min == 0)
                {
                    set_timer_min = 2;
                }
                else
                {
                    set_timer_min--;
                }
            }
            else if ((sw1_state != HW01_ALERT_NONE) && (set_set_timer_sec == true))
            {
                //  increment timer sec when sw1 pushed
                sw1_state = HW01_ALERT_NONE;
                if (set_timer_sec == 59)
                {
                    set_timer_sec = 0;
                }
                else
                {
                    set_timer_sec++;
                }
            }
            else if ((sw2_state != HW01_ALERT_NONE) && (set_set_timer_sec == true))
            {
                // decrement timer sec when sw2 pushed
                sw2_state = HW01_ALERT_NONE;
                if (set_timer_sec == 0)
                {
                    set_timer_sec = 59;
                }
                else
                {
                    set_timer_sec--;
                }
            }

            if (sw3_state == HW01_ALERT_BUTTON_GT_2S)
            {
                // if sw3 pushed more than 2sec, change to run mode
                sw3_state = HW01_ALERT_NONE;
                curr = RUN_MODE;
            }
            else if (sw3_state == HW01_ALERT_BUTTON_LT_2S)
            {
                // if pushed less than 2 sec, edit another variable in timer
                sw3_state = HW01_ALERT_NONE;
                set_set_timer_sec = !set_set_timer_sec;
            }

            if (blink_state == HW01_ALERT_BLINK)
            {
                // if blink state, display black color
                hw01_display_time(set_timer_min, set_timer_sec, LCD_COLOR_BLACK);
            }
            else
            {   // display normal color
                hw01_display_time(set_timer_min, set_timer_sec, HW01_LCD_TIME_COLOR);
            }
            hw01_display_alarm(0, 0, HW01_LCD_ALARM_COLOR);

            break;
        }

        case RUN_MODE:
        {
            // run mode, where you can start the alarm
            if (update_state == HW01_ALERT_TIME_UPDATE)
            {
                update_state = HW01_ALERT_NONE;
                // update normal timer
                if (set_timer_sec == 59)
                {
                    if (set_timer_min == 2)
                    {
                        set_timer_min = 0;
                        set_timer_sec = 0;
                    }
                    else
                    {
                        set_timer_min++;
                        set_timer_sec = 0;
                    }
                }   // max min 2, max sec 59
                else
                {
                    set_timer_sec++;
                }
            }
            else
            {
                set_timer_sec = set_timer_sec;
                set_timer_min = set_timer_min;
            }

            if (sw1_state == HW01_ALERT_BUTTON_GT_2S)
            {
                // erase bell , turn off buzzer when sw2 pushed more than 2 sec
                alarm_disable = true;
                hw01_erase_bell();
                pwm_buzzer_stop();
                alarm = false;
                sw1_state = HW01_ALERT_NONE;
            }
            else if (sw3_state == HW01_ALERT_BUTTON_GT_2S)
            {
                // when sw3 pushed more than 2 sec, change to alarm mode
                curr = SET_ALARM_MODE;
                sw3_state = HW01_ALERT_NONE;
            }
            else if (sw3_state == HW01_ALERT_BUTTON_LT_2S)
            {
                // when sw3 is pushed less than 2 sec, stop or turn on alarm
                sw3_state = HW01_ALERT_NONE;
                alarm = !alarm;
            }

            if (alarm)
            {
                // turn on the alarm
                hw01_draw_bell();
                if ((set_timer_min == alarm_set_timer_min) && (set_timer_sec == alarm_set_timer_sec))
                {
                    // once reach the alarm time, enable alarm
                    alarm_disable = false;
                }
            }
            else
            {
                // if alarm is off
                hw01_erase_bell();
                alarm_disable = true;
                pwm_buzzer_stop();
            }

            if (alarm_disable)
            {
                hw01_display_time(set_timer_min, set_timer_sec, HW01_LCD_TIME_COLOR);
                hw01_display_alarm(alarm_set_timer_min, alarm_set_timer_sec, HW01_LCD_ALARM_COLOR);
            }
            else
            {
                // when alarm is on, start buzzer, display timer in red
                hw01_display_time(set_timer_min, set_timer_sec, HW01_LCD_ALARM_ACTIVE_COLOR);
                hw01_display_alarm(alarm_set_timer_min, alarm_set_timer_sec, HW01_LCD_ALARM_COLOR);
                pwm_buzzer_start();
            }

            break;
        }

        case SET_ALARM_MODE:
        {
            // mode for setting alarm
            if (update_state == HW01_ALERT_TIME_UPDATE)
            {
                update_state = HW01_ALERT_NONE;
                // update normal timer
                if (set_timer_sec == 59)
                {
                    if (set_timer_min == 2)
                    {
                        set_timer_min = 0;
                        set_timer_sec = 0;
                    }
                    else
                    {
                        set_timer_min++;
                        set_timer_sec = 0;
                    }
                }
                else
                {
                    set_timer_sec++;
                }
            }

            // sw3 pushed less than 2 sec, edit different alarm variable
            if (sw3_state == HW01_ALERT_BUTTON_LT_2S)
            {
                sw3_state = HW01_ALERT_NONE;
                set_alarm_set_timer_sec = !set_alarm_set_timer_sec;
            }
            
            // if sw3 pushed more than 2 sec, change to run mode
            if (sw3_state == HW01_ALERT_BUTTON_GT_2S)
            {
                sw3_state = HW01_ALERT_NONE;
                curr = RUN_MODE;
            }

            // set alarm time as we do in the set timer mode
            if ((sw1_state == HW01_ALERT_BUTTON_GT_2S || sw1_state == HW01_ALERT_BUTTON_LT_2S) && set_alarm_set_timer_sec == false)
            {
                sw1_state = HW01_ALERT_NONE;
                if (alarm_set_timer_min == 2)
                {
                    alarm_set_timer_min = 0;
                }
                else
                {
                    alarm_set_timer_min++;
                }
            }
            else if ((sw2_state == HW01_ALERT_BUTTON_GT_2S || sw2_state == HW01_ALERT_BUTTON_LT_2S) && set_alarm_set_timer_sec == false)
            {
                sw2_state = HW01_ALERT_NONE;
                if (alarm_set_timer_min == 0)
                {
                    alarm_set_timer_min = 2;
                }
                else
                {
                    alarm_set_timer_min--;
                }
            }
            else if ((sw1_state == HW01_ALERT_BUTTON_GT_2S || sw1_state == HW01_ALERT_BUTTON_LT_2S) && set_alarm_set_timer_sec == true)
            {
                sw1_state = HW01_ALERT_NONE;
                if (alarm_set_timer_sec == 59)
                {
                    alarm_set_timer_sec = 0;
                }
                else
                {
                    alarm_set_timer_sec++;
                }
            }
            else if ((sw2_state == HW01_ALERT_BUTTON_GT_2S || sw2_state == HW01_ALERT_BUTTON_LT_2S) && set_alarm_set_timer_sec == true)
            {

                sw2_state = HW01_ALERT_NONE;
                if (alarm_set_timer_sec == 0)
                {
                    alarm_set_timer_sec = 59;
                }
                else
                {
                    alarm_set_timer_sec--;
                }
            }

            //blink the timer
            if (blink_state == HW01_ALERT_BLINK)
            {
                hw01_display_alarm(alarm_set_timer_min, alarm_set_timer_sec, LCD_COLOR_BLACK);
                hw01_display_time(set_timer_min, set_timer_sec, HW01_LCD_TIME_COLOR);
            }
            else
            {
                hw01_display_alarm(alarm_set_timer_min, alarm_set_timer_sec, HW01_LCD_ALARM_COLOR);
                hw01_display_time(set_timer_min, set_timer_sec, HW01_LCD_TIME_COLOR);
            }

            break;
        }
        default:
        {
            sw1_state = HW01_ALERT_NONE;
            sw2_state = HW01_ALERT_NONE;
            sw3_state = HW01_ALERT_NONE;
            curr = SET_TIME_MODE;
            break;
        }
        }
    }
}