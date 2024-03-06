/**
 * @file pwm-buzzer.c
 * @author Yining Wang (ywang2739@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "pwm-buzzer.h"

cyhal_pwm_t pwm_buzzer_obj;

void pwm_buzzer_init(void)
{
	cy_rslt_t rslt;

    // Initialize PWM on the supplied pin and assign a new clock
	rslt = cyhal_pwm_init(&pwm_buzzer_obj, PIN_IO_BUZZER, NULL);
}

void pwm_buzzer_start(void)
{
	cy_rslt_t rslt;

	// set duty and freq, then start the buzzer
	rslt = cyhal_pwm_set_duty_cycle(&pwm_buzzer_obj, PWM_BUZZER_DUTY, PWM_BUZZER_FREQ);
	rslt = cyhal_pwm_start(&pwm_buzzer_obj);
    
}

void pwm_buzzer_stop(void)
{
	cy_rslt_t rslt;

	// stop the buzzer
	rslt = cyhal_pwm_stop(&pwm_buzzer_obj);
}



