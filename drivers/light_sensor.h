#ifndef __LIGHT_SENSOR_H__
#define __LIGHT_SENSOR_H__


#include "io-expander.h"

uint16_t light_sensor(void);

#define LIGHT_SENSOR_ADDR                        0x29
#define LIGHT_SENSOR_CTRL                        0x80
#define LIGHT_SENSOR_RATE                        0x85
#define ALS_CH1_1_ADDR              0x89
#define ALS_CH1_0_ADDR              0x88
#define ALS_CH0_0_ADDR              0x8A
#define ALS_CH0_1_ADDR              0x8B
#define ALS_STATUS                  0x8C
#define ALS_STATUS_BIT              0x04
#define CTRL_CMD                    0x01 
#define MEASURE_T                   0x12


#endif