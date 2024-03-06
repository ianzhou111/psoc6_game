#ifndef __TASK_IMU_H__
#define __TASK_IMU_H_

#include "splatoon.h"

extern project_state state;

extern QueueHandle_t Queue_Stylus_Position;
void init_imu_task(void);

#endif