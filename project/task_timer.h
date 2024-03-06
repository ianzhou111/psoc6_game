#ifndef __TASK_TIMER_H__
#define __TASK_TIMER_H_

#include "splatoon.h"

typedef enum
{
    ALERT_TIMER_NONE,
    ALERT_TIMER_100MS
} alert_timer_t; // detects a alert of 100ms

#define TICKS_MS_100 10000000
#define game_time 20

extern project_state state;
extern QueueHandle_t timer_queue;

void init_timer_task(void);

#endif