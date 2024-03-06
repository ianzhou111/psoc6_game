#include "task_button.h"

void task_button(void *pvParameters);

//initialize button task here
void init_button_task(void){
    //printf("aaaaaaaaaaa\n");
    Queue_button = xQueueCreate(1, sizeof(button_state_t));

    xTaskCreate(task_button,
                "button",
                configMINIMAL_STACK_SIZE,
                NULL,
                5,
                NULL);
}


void task_button(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    button_state_t button_state = BUTTON_INACTIVE;

    //printf("aaaaaaaa\n");
    while (1)
    {
        vTaskDelay(5);
        button_state = get_buttons();

        //send button state to queue
        if (button_state != BUTTON_INACTIVE)
        {
           
            /* Send the current joystick position to the stylus position queue */
            xQueueSend(Queue_button, &button_state, portMAX_DELAY);
        }
    }
}