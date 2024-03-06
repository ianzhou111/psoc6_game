/**
 * @file splatoon.c
 * @author Ian Zhou
 * @brief
 * @version 0.1
 * @date 2023-12-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "splatoon.h"
#include "task_imu.h"
#include "task_lcd.h"
#include "task_timer.h"
#include "task_button.h"
#include "task_uart.h"

char project_DESCRIPTION[] = "ECE353: project Ian Zhou, Yining Wang";

// queues for tasks
QueueHandle_t Queue_button;
QueueHandle_t Queue_Stylus_Position;
QueueHandle_t timer_queue;
QueueHandle_t Queue_uart;

// initialize the 2d array mapping the board

struct Player cur_player;
int boundary_color = LCD_COLOR_CYAN; // default bg color

/*
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */

void project_peripheral_init(void)
{
    joystick_init();
    /* Initialize the pushbuttons */
    push_buttons_init();
    /* Initialize the LCD */
    ece353_enable_lcd();
    /* Initialize the remote UART */
    remote_uart_init();
    /* Initialize Timer to generate interrupts every 100mS*/
    i2c_init();
    
    // spi_init();
}

void draw_boundaries(void)
{

    lcd_draw_rectangle(0, SCREEN_X, 0, min_y, boundary_color); // top rectangle
    // lcd_draw_rectangle(0, SCREEN_X, SCREEN_Y, SCREEN_Y - max_y, boundary_color);

    lcd_draw_rectangle(0, min_x, 0, SCREEN_Y, boundary_color);
    lcd_draw_rectangle(max_x, SCREEN_X - max_x, 0, SCREEN_Y, boundary_color);
}

// default draw player function
void draw_players(void)
{
    lcd_draw_image(100, 100, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
    lcd_draw_image(100, 150, templerWidthPixels, templerHeightPixels, templerBitmaps, p2_color, BG_COLOR);
}

bool wait_for_start(void)
{
    lcd_select_player1(); // splash screen
    while (1)
    {
        // stuck in while loop until detect a start
        button_state_t curr = get_buttons();
        // if()
    }
    lcd_clear_screen(LCD_COLOR_BLACK);
}

void project_main_app(void)
{
    // project_peripheral_init();
    //   if (wait_for_start()){
    //      cur_player.p_num = 1;//p1 if this machine initialized start
    //   }else{
    //      cur_player.p_num = 2;
    //   }
    int dark = 0;
    uint8_t tx_msg[2];
    uint8_t rx_msg[2];
    uint16_t light = light_sensor();
    // printf("light: %d\n", light);
    cyhal_uart_putc(&cy_retarget_io_uart_obj, '\r');

    // detect the brightness and change the bg color
    if (light > 100)
    {
        dark = 1;
    }
    if (dark)
        boundary_color = LCD_COLOR_YELLOW;
    else
        boundary_color = LCD_COLOR_CYAN;
    draw_boundaries(); // draw the bounds

    while (1)
    {
        button_state_t curr = get_buttons();
        if (curr == BUTTON_SW1_RELEASED) // if sw1 pressed, it should be player one
        {
            cur_player.p_num = 1;
            tx_msg[0] = 111;
            tx_msg[1] = '\n';
            remote_uart_tx_string_polling(tx_msg);
            break;
            // remote_uart_tx_string_polling("\n");
        }
        else if (remote_uart_rx_string_polling(rx_msg))
        {
            // printf("1\n");
            // printf("%x\n\r", rx_msg[0]);
            // printf("%x\n\r", rx_msg[1]);
            if (rx_msg[0] == 111)
            {
                memset(rx_msg, 0, 2);
                cur_player.p_num = 2;
                cur_player.p_color = LCD_COLOR_BLUE;
                break;
            }
        }
    }
    // printf("init1\n");
    init_imu_task();
    // write about moving assuming that player is selected to be one
    // printf("init2\n");
    /* Create the Queue for the Stylus Position */
    init_button_task();
    // printf("init3\n");
    init_lcd_task();
    // printf("init4\n");
    init_uart_task();

    init_timer_task();
    // printf("init\n");
    eeprom_cs_init();
    spi_init();
    init_led_task();
    vTaskStartScheduler();
    while (1)
    {
    };
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while (1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    // Handle FreeRTOS Stack Overflow
    while (1)
    {
    }
}