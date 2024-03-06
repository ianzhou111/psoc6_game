#include "task_uart.h"

void task_uart(void *pvParameters);

void init_uart_task(void)
{
    remote_uart_enable_interrupts();
    printf("aaaaaaaaaaa\n");
    Queue_uart = xQueueCreate(1, 20);
    printf("queue suc");
    xTaskCreate(task_uart,
                "uart",
                configMINIMAL_STACK_SIZE,
                NULL,
                6,
                NULL);
}

void task_uart(void *pvParameters)
{

    uint8_t rx_msg[2];
    char remote_rx_message[10];
    while (1)
    {
        if (ALERT_UART_RX)
        {
            /* Set the alert back to false*/
            ALERT_UART_RX = false;

            /* Initialize the array to all 0*/
            memset(remote_rx_message, 0, 10);

            if (remote_uart_rx_data_async(remote_rx_message, 10))
            {
                // restart check, if rstr is received restart the game
                if (remote_rx_message[0] == 'R' && remote_rx_message[1] == 'S' && remote_rx_message[2] == 'T' && remote_rx_message[3] == 'R')
                {
                    struct Index index;
                    index.valid = -1;                               // indicated restart
                    xQueueSend(Queue_uart, &index, portMAX_DELAY); // send to the queue
                }
                else
                {
                    // translate the recieved char to index and send to the queue
                    int x = 10 * (remote_rx_message[0] - '0') + (remote_rx_message[1] - '0');
                    int y = 10 * (remote_rx_message[2] - '0') + (remote_rx_message[3] - '0');
                    /* print out the message received from the remote uart */
                    struct Index index;
                    index.valid = 1;
                    index.x_arr_index = x;
                    index.y_arr_index = y;
                    index.x_scr_index = 24 + x * 16;
                    index.y_scr_index = 72 + index.y_arr_index * 16;
                    printf("received: int %d %d\n\r", x, y);
                    xQueueSend(Queue_uart, &index, portMAX_DELAY);
                }
            }
        }

        // if (remote_uart_rx_string_polling(rx_msg))
        // {
        //     struct Index index;
        //     index.valid = 1;
        //     index.x_arr_index = rx_msg[0];
        //     printf("%d\n", rx_msg[0]);
        //     index.y_arr_index = rx_msg[1];
        //     index.x_scr_index = 24 + index.x_arr_index * 16;
        //     index.y_scr_index = 72 + index.y_arr_index * 16;
        //     xQueueSend(Queue_uart, &index, portMAX_DELAY);
        //     index.valid = 0;
        //     memset(rx_msg, 0, 2);
        // }
        else
        {
            struct Index index;
            index.valid = 0;
            xQueueSend(Queue_uart, &index, portMAX_DELAY);
        }
        // }
    }
}