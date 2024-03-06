/**
 * @file hw01_timer.c
 * @author Yining Wang (ywang2739@wisc.edu)
 * @author Ian Zhou (izhou3@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hw01_timer.h"
/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/
void Handler_HW01_Timer(void *handler_arg, cyhal_timer_event_t event);

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
hw01_timer_alerts_t sw1_state = HW01_ALERT_NONE;
hw01_timer_alerts_t sw2_state = HW01_ALERT_NONE;
hw01_timer_alerts_t sw3_state = HW01_ALERT_NONE;
hw01_timer_alerts_t blink_state = HW01_ALERT_NONE;
hw01_timer_alerts_t update_state = HW01_ALERT_NONE;

/* Timer object and timer_cfg object */
cyhal_timer_t timer_obj;

cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,              /* Timer compare value, not used */
        .period = 0,                     /* number of timer ticks */
        .direction = CYHAL_TIMER_DIR_UP, /* Timer counts up */
        .is_compare = false,             /* Don't use compare mode */
        .is_continuous = true,           /* Run timer indefinitely */
        .value = 0                       /* Initial value of counter */
};

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/

/**
 * @brief
 *  Function used as Timer Handler
 */
void Handler_HW01_Timer(void *handler_arg, cyhal_timer_event_t event)
{
    CY_UNUSED_PARAMETER(handler_arg);
    CY_UNUSED_PARAMETER(event);

    uint32_t read = PORT_BUTTONS->IN; // read in buttons
    static int update_count = 0;      // count for update time, should count to 10 (1hz)
    static int blink_count = 0;       // count for blink, count to 5 2hz
    static int sw1_count = 0;         // count for buttons, determine whether pushsed over 2 secs
    static int sw2_count = 0;
    static int sw3_count = 0;

    static bool sw1_curr = false; // static variables for recording cur and prev states for each button
    static bool sw2_curr = false;
    static bool sw3_curr = false;

    static bool sw1_prev = false;
    static bool sw2_prev = false;
    static bool sw3_prev = false;

    // when sw1 is pushed
    if ((read & SW1_MASK) == 0)
    {
        sw1_curr = true;
        sw1_count++;
    }
    else
    {
        sw1_curr = false;
    }

    // when sw2 is pushed
    if ((read & SW2_MASK) == 0)
    {
        sw2_curr = true;
        sw2_count++;
    }
    else
    {
        sw2_curr = false;
    }

    // when sw3 is pushed
    if ((read & SW3_MASK) == 0)
    {
        sw3_curr = true;
        sw3_count++;
    }
    else
    {
        sw3_curr = false;
    }

    // when sw3 is released
    if (!sw3_curr && sw3_prev)
    {
        if (sw3_count < 20)
        {
            sw3_state = HW01_ALERT_BUTTON_LT_2S; // when sw3 is pushed less than 2 sec
        }
        else
        {
            sw3_state = HW01_ALERT_BUTTON_GT_2S; // when sw3 is pushed more than 2 sec
        }
        sw3_count = 0; // reset count
    }
    else
    {
        sw3_state = HW01_ALERT_NONE;
    }

    // when sw2 is released
    if (!sw2_curr && sw2_prev)
    {

        if (sw2_count < 20)
        {

            sw2_state = HW01_ALERT_BUTTON_LT_2S; // when sw2 pushed less than 2 sec
        }
        else
        {
            sw2_state = HW01_ALERT_BUTTON_GT_2S; // when sw2 pushed more than 2 sec
        }
        sw2_count = 0;// reset count
    }
    else
    {
        sw2_state = HW01_ALERT_NONE;
    }

    // when sw1 released
    if (!sw1_curr && sw1_prev)
    {
        if (sw1_count < 20)
        {
            sw1_state = HW01_ALERT_BUTTON_LT_2S; // when sw1 pushed less than 2 sec
        }
        else
        {
            sw1_state = HW01_ALERT_BUTTON_GT_2S; // when sw1 pushed more than 2 sec
        }
        sw1_count = 0;
    }
    else
    {
        sw1_state = HW01_ALERT_NONE;
    }

    // update current state to prev
    sw1_prev = sw1_curr;
    sw2_prev = sw2_curr;
    sw3_prev = sw3_curr;

    // update time at 1 hz
    if (update_count == 9)
    {
        update_state = HW01_ALERT_TIME_UPDATE;
        update_count = 0;
    }
    else
    {
        update_state = HW01_ALERT_NONE;
        update_count++;
    }

    // blink at 2 hz
    if (blink_count < 5)
    {
        blink_state = HW01_ALERT_BLINK;
    }
    else
    {
        blink_state = HW01_ALERT_NONE;
    }
    blink_count = (blink_count + 1) % 10;
}

/*****************************************************************************/
/*  TCPWM Initialization Function                                            */
/*****************************************************************************/
void hw01_timer_init(uint32_t ticks)
{
    timer_cfg.period = ticks;
    cy_rslt_t rslt;

    /* Initialize a timer */
    rslt = cyhal_timer_init(&timer_obj, NC, NULL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU

    /* Apply timer configuration such as period, count direction, run mode, etc. */
    rslt = cyhal_timer_configure(&timer_obj, &timer_cfg);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU

    /* Set the frequency of timer*/
    rslt = cyhal_timer_set_frequency(&timer_obj, 100000000);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU

    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&timer_obj, Handler_HW01_Timer, NULL);

    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

    /* Start the timer with the configured settings */
    rslt = cyhal_timer_start(&timer_obj);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
}
