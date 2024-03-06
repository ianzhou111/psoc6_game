#include "main.h"
#include "systick.h"
#include "task_timer.h"
volatile alert_timer_t ALERT_TIMER = ALERT_TIMER_NONE;
cyhal_timer_t hw2_timer_obj;
cyhal_timer_cfg_t hw2_timer_cfg;
extern cyhal_uart_t cy_retarget_io_uart_obj;
int second = 0;
int playtime = game_time;

void task_timer(void *pvParameters);

// timer handler
void Handler_Timer(void *handler_arg, cyhal_timer_event_t event)
{
    ALERT_TIMER = ALERT_TIMER_100MS;
}

void init_timer_task(void){
    timer_init(&hw2_timer_obj, &hw2_timer_cfg, TICKS_MS_100, Handler_Timer);
    timer_queue = xQueueCreate(1, sizeof(int));

    xTaskCreate(task_timer,
                "lcd",
                configMINIMAL_STACK_SIZE,
                NULL,
                4,
                NULL);
}



// send info to queue when counts 10 ticks which is 1 sec
void task_timer(void *pvParameters)
{
    
    while (1)
    {
        if (ALERT_TIMER == ALERT_TIMER_100MS)
        {
            ALERT_TIMER = ALERT_TIMER_NONE;

            if (second == 0)
            {
                xQueueSend(timer_queue, &playtime, portMAX_DELAY); //if one sec, send the cur time to queue
                if(playtime>0){
                    playtime--;
                }
                
            } else{
                int fal = -1;
                xQueueSend(timer_queue, &fal, portMAX_DELAY); // send nonvalid to queue
            }
            second = (second + 1) % 10;
        }
    }
}
