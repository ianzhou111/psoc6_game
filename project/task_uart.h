#ifndef __TASK_UART_H__
#define __TASK_UART_H_

#include "splatoon.h"


extern QueueHandle_t Queue_uart;
void init_uart_task(void);

#endif