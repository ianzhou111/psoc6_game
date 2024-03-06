#include "task_lcd.h"
#include "task_imu.h"
#include "task_timer.h"
#include "task_button.h"
#include "task_uart.h"

project_state state = start;
board_status board[grid_x + 1][grid_y + 1];
void task_lcd(void *pvParameters);
int high_score = 0;
uint16_t address = 0x1342;
int need_won = 1;

int firead = 0;

void task_buzzer_stop(void *pvParameters);
cyhal_gpio_callback_data_t sw3_callback_data;
TaskHandle_t buzzer_stop;

void sw3_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
     it will get set to pdTRUE inside the interrupt safe API function if a
     context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* ADD CODE */
    /* Send a notification directly to the task to which interrupt processing
     * is being deferred.
     */
    vTaskNotifyGiveFromISR(buzzer_stop, &xHigherPriorityTaskWoken);

    /* ADD CODE */
    /* Call the function that will force the task that was running prior to the
     * interrupt to yield*/
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// sw3 is used to interrupt
void sw3_irq_enable(void)
{
    sw3_callback_data.callback = sw3_handler;
    cyhal_gpio_register_callback(PIN_SW3, &sw3_callback_data);
    cyhal_gpio_enable_event(PIN_SW3, CYHAL_GPIO_IRQ_FALL, 3, true);
}

// initialize task
void init_lcd_task(void)
{
    pwm_buzzer_init();
    sw3_irq_enable();
    xTaskCreate(task_lcd,
                "lcd",
                configMINIMAL_STACK_SIZE * 5,
                NULL,
                3,
                NULL);
    xTaskCreate(task_buzzer_stop,
                "bs",
                configMINIMAL_STACK_SIZE,
                NULL,
                3,
                &buzzer_stop);
}

// function used to check who wins the game
int who_won(void)
{
    int p1 = 0;
    int p2 = 0;
    for (int i = 0; i < grid_x + 1; i++)
    {
        for (int j = 0; j < grid_y + 1; j++)
        {
            if (board[i][j] == P1_CLAIM)
                p1++;
            if (board[i][j] == P2_CLAIM)
                p2++;
        }
    }
    printf("p1 = %d , p2 = %d \n", p1, p2);
    if (p2 == p1)
    {
        high_score = store_high_score(p1); // use eeprom to store the high score
        return 0;
    }
    else if (p1 > p2)
    {
        high_score = store_high_score(p1);
        return 1;
    }
    else
    {
        high_score = store_high_score(p2);
        return 2;
    }
}

// a dumb function which translate the input int to chars
// for some reason, functions like sprintf does not work
void digit_to_char(int index, char *arr)
{
    if (index % 10 == 0)
        arr[1] = '0';
    else if (index % 10 == 1)
        arr[1] = '1';
    else if (index % 10 == 2)
        arr[1] = '2';
    else if (index % 10 == 3)
        arr[1] = '3';
    else if (index % 10 == 4)
        arr[1] = '4';
    else if (index % 10 == 5)
        arr[1] = '5';
    else if (index % 10 == 6)
        arr[1] = '6';
    else if (index % 10 == 7)
        arr[1] = '7';
    else if (index % 10 == 8)
        arr[1] = '8';
    else if (index % 10 == 9)
        arr[1] = '9';

    if (index >= 10)
    {
        arr[0] = '1';
    }
    else
    {
        arr[0] = '0';
    }
}

/**
 * @brief
 * lcd function, which also acts as a state machine
 *
 * @param pvParameters
 */
void task_lcd(void *pvParameters)
{

    playtime = game_time; // initialize playtime
    firead = eeprom_read_byte(address);
    high_score = firead;
    hw01_display_highscore(high_score);


    struct Index player1_index; // player index 
    struct Index player2_index;
    struct Index player2_pindex; // record previous player idnex
    struct Index player1_pindex;
    if (cur_player.p_num == 1)
    {
        player1_index.x_arr_index = 1;
        player1_index.x_scr_index = 40;
        player1_index.y_arr_index = 5;
        player1_index.y_scr_index = 152;
        player2_index.x_arr_index = 16;
        player2_index.x_scr_index = 280;
        player2_index.y_arr_index = 5;
        player2_index.y_scr_index = 152;

        player1_pindex.x_arr_index = 1;
        player1_pindex.x_scr_index = 40;
        player1_pindex.y_arr_index = 5;
        player1_pindex.y_scr_index = 152;
        player2_pindex.x_arr_index = 16;
        player2_pindex.x_scr_index = 280;
        player2_pindex.y_arr_index = 5;
        player2_pindex.y_scr_index = 152;
    }

    if (cur_player.p_num == 2)
    {
        player2_index.x_arr_index = 1;
        player2_index.x_scr_index = 40;
        player2_index.y_arr_index = 5;
        player2_index.y_scr_index = 152;
        player1_index.x_arr_index = 16;
        player1_index.x_scr_index = 280;
        player1_index.y_arr_index = 5;
        player1_index.y_scr_index = 152;

        player2_pindex.x_arr_index = 1;
        player2_pindex.x_scr_index = 40;
        player2_pindex.y_arr_index = 5;
        player2_pindex.y_scr_index = 152;
        player1_pindex.x_arr_index = 16;
        player1_pindex.x_scr_index = 280;
        player1_pindex.y_arr_index = 5;
        player1_pindex.y_scr_index = 152;
    }  // assign the positions to player index by player selection

    BaseType_t imu_status;
    BaseType_t sw_status;
    BaseType_t timer_status;
    BaseType_t uart_status;

    uint16_t new_color = LCD_COLOR_GREEN;
    int second;
    imu_dir pos = CENTER;
    button_state_t button;

    state = run;
    // playtime = game_time;
   
    // draw initial position for two players
    if (cur_player.p_num == 1)
    {
        lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
        board[player1_index.x_arr_index][player1_index.y_arr_index] = P1_CLAIM;
        lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
        board[player2_index.x_arr_index][player2_index.y_arr_index] = P2_CLAIM;
    }
    else
    {
        lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
        board[player1_index.x_arr_index][player1_index.y_arr_index] = P2_CLAIM;
        lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
        board[player2_index.x_arr_index][player2_index.y_arr_index] = P1_CLAIM;
    }

    while (1)
    {
        // receive info from queue
        uart_status = xQueueReceive(Queue_uart, &player2_index, 5);
        imu_status = xQueueReceive(Queue_Stylus_Position, &pos, 5);
        timer_status = xQueueReceive(timer_queue, &second, 5);
        sw_status = xQueueReceive(Queue_button, &button, 5);

        // in the run state, receive timing, imu and uart queue data
        if (state == run)
        {

            // if valid number received from the queue, display the updated timer
            if (timer_status != NULL && second != -1)
            {
                if (second == 0)
                {
                    hw01_display_alarm(0, second, LCD_COLOR_GREEN);
                    state = idle;
                    pwm_buzzer_start(); // start buzzer when game ends, and jump to the idle state
                    continue;
                }
                hw01_display_alarm(0, second, LCD_COLOR_GREEN);
            }

            // read from the uart and if the index received is valid
            if (uart_status != NULL && player2_index.valid)
                {
                    // by cur playuer pnum
                    if (cur_player.p_num == 1)
                    {
                        /* Draw the cursor's initial position */
                        lcd_draw_rectangle(player2_pindex.x_scr_index - 8, 16, player2_pindex.y_scr_index - 8, 16, p2_color);
                        lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);

                        player2_pindex.x_scr_index = player2_index.x_scr_index;
                        player2_pindex.y_scr_index = player2_index.y_scr_index;
                        player2_pindex.y_arr_index = player2_index.y_arr_index;
                        player2_pindex.x_arr_index = player2_index.x_arr_index;

                        board[player2_index.x_arr_index][player2_index.y_arr_index] = P2_CLAIM; // draw the updated other player position and store it to the board array
                    }
                    else
                    {
                        lcd_draw_rectangle(player2_pindex.x_scr_index - 8, 16, player2_pindex.y_scr_index - 8, 16, p1_color);
                        lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);

                        player2_pindex.x_scr_index = player2_index.x_scr_index;
                        player2_pindex.y_scr_index = player2_index.y_scr_index;
                        player2_pindex.y_arr_index = player2_index.y_arr_index;
                        player2_pindex.x_arr_index = player2_index.x_arr_index;

                        board[player2_index.x_arr_index][player2_index.y_arr_index] = P1_CLAIM;  // draw the updated other player position and store it to the board array
                    }
                }

            // receive imu queue
            if (imu_status != NULL && pos != CENTER)
            {
                // draw the new position for each player
                if (cur_player.p_num == 1)
                {
                    lcd_draw_rectangle(player1_index.x_scr_index - 8, 16, player1_index.y_scr_index - 8, 16, p1_color);
                }
                else
                {
                    lcd_draw_rectangle(player1_index.x_scr_index - 8, 16, player1_index.y_scr_index - 8, 16, p2_color);
                }
                
                // by the index received, calculate the player position of moved position
                switch (pos)
                {
                case LEFT:
                {
                    if (player1_index.x_arr_index > 0)
                    {
                        player1_index.x_arr_index--;
                        player1_index.x_scr_index -= 16;
                    }

                    break;
                }
                case RIGHT:
                {
                    if (player1_index.x_arr_index < grid_x)
                    {
                        player1_index.x_arr_index++;
                        player1_index.x_scr_index += 16;
                    }
                    break;
                }
                case UP:
                {
                    if (player1_index.y_arr_index > 0)
                    {
                        player1_index.y_arr_index--;
                        player1_index.y_scr_index -= 16;
                    }
                    break;
                }
                case DOWN:
                {
                    if (player1_index.y_arr_index < grid_y)
                    {
                        player1_index.y_arr_index++;
                        player1_index.y_scr_index += 16;
                    }
                    break;
                }
                }

                
                // use async uart to send new position to the other plyaer
                char tx_msg_x[2];
                digit_to_char(player1_index.x_arr_index, tx_msg_x);
                char tx_msg_y[3];
                tx_msg_y[2] = '\0';
                digit_to_char(player1_index.y_arr_index, tx_msg_y);
                char tx_msg[5] = {tx_msg_x[0], tx_msg_x[1], tx_msg_y[0], tx_msg_y[1], tx_msg_y[2]};
                printf("tx %s\n", tx_msg);
                remote_uart_tx_data_async(tx_msg);

                /* Need to send a \r or \n to indicate the end of a string*/
                remote_uart_tx_char_async('\n');

                if (cur_player.p_num == 1)
                {
                    // draw the calculated new position
                    lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                    board[player1_index.x_arr_index][player1_index.y_arr_index] = P1_CLAIM;
                }
                else
                {
                    lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
                    board[player1_index.x_arr_index][player1_index.y_arr_index] = P2_CLAIM;
                }
            }
        }
        else // idle state
        { 
            int winner;
            if (need_won)
            {
                winner = who_won(); // check who wins
                need_won = 0;
            }
            // draw the winner
            if (winner == 0)
            {
                lcd_tie(); 
            }
            else if (winner == 1)
            {
                lcd_X_wins();
            }
            else
            {
                lcd_O_wins();
            }
            if (sw_status != NULL)
            {
                if (button == BUTTON_SW2_RELEASED)
                {
                    need_won = 1;
                    playtime = game_time;
                    // if sw2 press, initialize all var and return to run state, restart everything
                    char tx_msg[5] = {'R', 'S', 'T', 'R', '\0'};
                    remote_uart_tx_data_async(tx_msg);
                    /* Need to send a \r or \n to indicate the end of a string*/
                    remote_uart_tx_char_async('\n');
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    // lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                    draw_boundaries();
                    state = run;
                    memset(board, -1, sizeof(board));
                    hw01_display_highscore(high_score);
                    if (cur_player.p_num == 1)
                    {
                        player1_index.x_arr_index = 1;
                        player1_index.x_scr_index = 40;
                        player1_index.y_arr_index = 5;
                        player1_index.y_scr_index = 152;
                        player2_index.x_arr_index = 16;
                        player2_index.x_scr_index = 280;
                        player2_index.y_arr_index = 5;
                        player2_index.y_scr_index = 152;

                        player1_pindex.x_arr_index = 1;
                        player1_pindex.x_scr_index = 40;
                        player1_pindex.y_arr_index = 5;
                        player1_pindex.y_scr_index = 152;
                        player2_pindex.x_arr_index = 16;
                        player2_pindex.x_scr_index = 280;
                        player2_pindex.y_arr_index = 5;
                        player2_pindex.y_scr_index = 152;
                    }

                    if (cur_player.p_num == 2)
                    {
                        player2_index.x_arr_index = 1;
                        player2_index.x_scr_index = 40;
                        player2_index.y_arr_index = 5;
                        player2_index.y_scr_index = 152;
                        player1_index.x_arr_index = 16;
                        player1_index.x_scr_index = 280;
                        player1_index.y_arr_index = 5;
                        player1_index.y_scr_index = 152;

                        player2_pindex.x_arr_index = 1;
                        player2_pindex.x_scr_index = 40;
                        player2_pindex.y_arr_index = 5;
                        player2_pindex.y_scr_index = 152;
                        player1_pindex.x_arr_index = 16;
                        player1_pindex.x_scr_index = 280;
                        player1_pindex.y_arr_index = 5;
                        player1_pindex.y_scr_index = 152;
                    }
                    if (cur_player.p_num == 1)
                    {
                        /* Draw the cursor's initial position */
                        lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                        board[player1_index.x_arr_index][player1_index.y_arr_index] = P1_CLAIM;
                        lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
                        board[player2_index.x_arr_index][player2_index.y_arr_index] = P2_CLAIM;
                    }
                    else
                    {
                        lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
                        board[player1_index.x_arr_index][player1_index.y_arr_index] = P2_CLAIM;
                        lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                        board[player2_index.x_arr_index][player2_index.y_arr_index] = P1_CLAIM;
                    }
                }
            }
            if ((uart_status != NULL) && (player2_index.valid == -1))
            { 
                // receive the sw2 pressed info from other player and restart the game
                need_won = 1;
                playtime = game_time;
                printf("sbsbsbbsb\n");
                lcd_clear_screen(LCD_COLOR_BLACK);
                // lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                draw_boundaries();
                state = run;
                memset(board, -1, sizeof(board));
                hw01_display_highscore(high_score);
                if (cur_player.p_num == 1)
                {
                    player1_index.x_arr_index = 1;
                    player1_index.x_scr_index = 40;
                    player1_index.y_arr_index = 5;
                    player1_index.y_scr_index = 152;
                    player2_index.x_arr_index = 16;
                    player2_index.x_scr_index = 280;
                    player2_index.y_arr_index = 5;
                    player2_index.y_scr_index = 152;

                    player1_pindex.x_arr_index = 1;
                    player1_pindex.x_scr_index = 40;
                    player1_pindex.y_arr_index = 5;
                    player1_pindex.y_scr_index = 152;
                    player2_pindex.x_arr_index = 16;
                    player2_pindex.x_scr_index = 280;
                    player2_pindex.y_arr_index = 5;
                    player2_pindex.y_scr_index = 152;
                }

                if (cur_player.p_num == 2)
                {
                    player2_index.x_arr_index = 1;
                    player2_index.x_scr_index = 40;
                    player2_index.y_arr_index = 5;
                    player2_index.y_scr_index = 152;
                    player1_index.x_arr_index = 16;
                    player1_index.x_scr_index = 280;
                    player1_index.y_arr_index = 5;
                    player1_index.y_scr_index = 152;

                    player2_pindex.x_arr_index = 1;
                    player2_pindex.x_scr_index = 40;
                    player2_pindex.y_arr_index = 5;
                    player2_pindex.y_scr_index = 152;
                    player1_pindex.x_arr_index = 16;
                    player1_pindex.x_scr_index = 280;
                    player1_pindex.y_arr_index = 5;
                    player1_pindex.y_scr_index = 152;
                }

                if (cur_player.p_num == 1)
                {
                    /* Draw the cursor's initial position */
                    lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                    board[player1_index.x_arr_index][player1_index.y_arr_index] = P1_CLAIM;
                    lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
                    board[player2_index.x_arr_index][player2_index.y_arr_index] = P2_CLAIM;
                }
                else
                {
                    lcd_draw_image(player1_index.x_scr_index, player1_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p2_color, BG_COLOR);
                    board[player1_index.x_arr_index][player1_index.y_arr_index] = P2_CLAIM;
                    lcd_draw_image(player2_index.x_scr_index, player2_index.y_scr_index, illWidthPixels, illHeightPixels, illBitmaps, p1_color, BG_COLOR);
                    board[player2_index.x_arr_index][player2_index.y_arr_index] = P1_CLAIM;
                }
            }
        }
    }
}

int Write_Values;

// use eeprom to store the highest score of the entire playtime
int store_high_score(int new_high)
{
    // printf("new high: %d\n",new_high);
    int read_byte;
    read_byte = eeprom_read_byte(address);
    printf("read byte %d \n", read_byte);
    // printf("read byte: %d\n",read_byte);
    if (read_byte >= new_high)
    {
        return read_byte;
    }
    eeprom_write_byte(address, new_high);
    return new_high;
    // printf("New High score\n");
}

// stop buzzer function, could be notified by sw3
void task_buzzer_stop(void *pvParameters)
{
    while (1)
    {
        ulTaskNotifyTake(true, portMAX_DELAY);
        pwm_buzzer_stop();
    }
}