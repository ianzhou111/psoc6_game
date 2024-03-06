#include "task_imu.h"

void task_imu(void *pvParameters);

//initialize imu task here
void init_imu_task(void){
    Queue_Stylus_Position = xQueueCreate(1, sizeof(imu_dir));

    xTaskCreate(task_imu,
                "Joystick",
                configMINIMAL_STACK_SIZE,
                NULL,
                2,
                NULL);
}


void task_imu(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    imu_dir dir = CENTER;

    while (1)
    {
        int16_t board_tilt[2];
        lsm6dsm_orientation(board_tilt); // get board space position
       

        // check the board position
        if (board_tilt[0] > 3500)
        {
            dir = LEFT;
            if(board_tilt[0]>5000){
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY); // send three straight dir to queue if tilt>5000, after test
                                                                        // this will make the obj moves 2 times faster (skips the lsm proc)
            }
            xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
        } 

        // if the board is tilt to right
        else if(board_tilt[0] < -3500)
        {
            dir = RIGHT;
            if(board_tilt[0]<-5000){
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
            }
            xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
        }
        // if the board is facing up
        else if(board_tilt[1] < -3500)
        {
            dir = UP;
            if(board_tilt[1]<-5000){
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
            }
            xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
        }
        // if the board is tilt down
        else if(board_tilt[1] > 3500)
        {
            dir = DOWN;
             if(board_tilt[1]>5000){
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
                xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
            }
            xQueueSend(Queue_Stylus_Position, &dir, portMAX_DELAY);
        }
    }
}