#include "task_lcd.h"
#include "task_imu.h"
#include "task_timer.h"
#include "task_button.h"
#include "task_uart.h"

void task_led(void *pvParameters);


    void init_led_task(void)
{
    xTaskCreate(task_led,
                "led",
                configMINIMAL_STACK_SIZE,
                NULL,
                2,
                NULL);

}




    void task_led(void *pvParameters){
    printf("enter task led\n");
    uint8_t active_led = 0x01;
    uint8_t io_sw = 0xFF;
    io_expander_set_configuration(0x80); 	// Set bit 7 as input, bits 6-0 as outputs 
    vTaskDelay(30);
	io_expander_set_output_port(0x00); 	
     printf("configure\n");	// Turn OFF all LEDs

    while(1)
    {

        /* Need to read the IO Expander due to the way Interrupts from the IO Expander work */
        // io_sw = io_expander_get_input_port();
        // printf("get io sw\n");

        /* Set the IO Expander Outputs to the active LED */ 
	    io_expander_set_output_port(active_led); 
        printf("set output port\n");	

        vTaskDelay(150);
        printf("active led value: %d\n",active_led);
        /* Rotate the active LED */
        if(active_led == 0x40)
        {
            printf("active led\n");
            active_led = 0x01;
        }
        else
        {
            printf("active led2\n");
            active_led = active_led << 1;
        }
        // if(state==run){
        //     vTaskDelete(NULL);
        // }

    }
    



    }