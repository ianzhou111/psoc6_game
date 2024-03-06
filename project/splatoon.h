/**
 * @file hw02.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __splatoon_H__
#define __splatoon_H__
#include "main.h"
#include "drivers/joystick.h"

#define SCREEN_X            320
#define SCREEN_Y            240

#define SCREEN_CENTER_COL    ((SCREEN_X/2)-1)
#define SCREEN_CENTER_ROW    ((SCREEN_Y/2)-1)

#define LINE_WIDTH          4
#define LINE_LENGTH         120 

#define SQUARE_SIZE         32

#define PADDING             2

#define LEFT_COL                      (SCREEN_CENTER_COL - SQUARE_SIZE - (2*PADDING) - LINE_WIDTH)
#define CENTER_COL                    (SCREEN_CENTER_COL)
#define RIGHT_COL                     (SCREEN_CENTER_COL + SQUARE_SIZE + (2*PADDING) + LINE_WIDTH)

#define UPPER_ROW                     (SCREEN_CENTER_ROW - SQUARE_SIZE - (2*PADDING) - LINE_WIDTH + 20 )
#define CENTER_ROW                    (SCREEN_CENTER_ROW + 20 )
#define LOWER_ROW                     (SCREEN_CENTER_ROW + SQUARE_SIZE + (2*PADDING) + LINE_WIDTH + 20 )

#define UPPER_HORIZONTAL_LINE_Y     (SCREEN_CENTER_ROW - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2 + 20)
#define LOWER_HORIZONTAL_LINE_Y     (SCREEN_CENTER_ROW + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2 + 20)

#define LEFT_VERTICAL_LINE_X      (SCREEN_CENTER_COL - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2)
#define RIGHT_VERTICAL_LINE_X     (SCREEN_CENTER_COL + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2)

#define LEFT_HORIZONTAL_LINE_X      (SCREEN_CENTER_COL - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2)
#define RIGHT_HORIZONTAL_LINE_X     (SCREEN_CENTER_COL + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2)

#define MESSAGE_PLAYER1_SELECTION 0x5A
#define MESSAGE_ACK               0xF0
#define MESSAGE_NACK              0xE0
#define MESSAGE_X                 0x58
#define MESSAGE_O                 0x4F
#define MESSAGE_RESTART           0x30

typedef enum
{
    EVENT_NONE,
    EVENT_100MS,
    EVENT_SW1,
    EVENT_SW2
} events_t;

typedef enum
{
    CENTER,
    RIGHT,
    LEFT,
    UP,
    DOWN
} imu_dir;

// Define states
typedef enum {
    IDLE,
    SEND,
    RECEIVE,
    UPDATE
} State;

#define pixel_size 16
#define min_x 16
#define min_y 64
#define max_x 304
#define max_y 224
extern int boundary_color;
#define p1_color LCD_COLOR_RED
#define p2_color LCD_COLOR_BLUE
#define BG_COLOR LCD_COLOR_BLACK

typedef enum {
PLAYER1,
PLAYER2,//player 2 on it 
EMPTY,
P1_CLAIM,//claimed by p1 
P2_CLAIM
} board_status;

extern cyhal_uart_t cy_retarget_io_uart_obj;
struct Index{
    int x_arr_index;
    int x_scr_index;
    int y_arr_index;
    int y_scr_index;
    int valid;
};

struct Player {
    int p_num;
    int p_color;
};


/*****************************************************************************/
/*  FreeRTOS Handles
/*****************************************************************************/
//QueueHandle_t Queue_Stylus_Position;
//QueueHandle_t Queue_Stylus_Color;
/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
// void task_imu(void *pvParameters);
// void task_lcd(void *pvParameters);
// void task_button_sw1(void *pvParameters);

#define grid_x 17
#define grid_y 10

#define LCD_MIN_X (2)
#define LCD_MAX_X (319 - 2)
#define LCD_MIN_Y (2)
#define LCD_MAX_Y (240 - 2)

#define CURSOR_SIZE 5 // change this 

#define TICKS_MS_100 (10000000)

typedef enum
{
    start,//default mode
    run, // start to run 
    idle, // wait for next match
} project_state;


/**
 * @brief 
 * Initializes the PSoC6 Peripherals used for HW02 
 */
void project_peripheral_init(void);

/**
 * @brief 
 * Implements the main application for HW02 
 */
void project_main_app(void);

#endif