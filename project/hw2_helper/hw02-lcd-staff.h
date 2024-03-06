/**
 * @file hw02-lcd-staff.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-10-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __HW02_LCD_STAFF_H__
#define __HW02_LCD_STAFF_H__

#include "drivers/io-lcd.h"
#include "hw02-images.h"

void tic_tac_toe_example_board(void);
void lcd_X_wins(void);
void lcd_O_wins(void);
void lcd_tie(void);
void lcd_select_player1(void);
void lcd_wait_for_other_player(void);
void lcd_clear_other_player(void);
void tic_tac_toe_example_board(void);

void lcd_draw_rectangle_centered(
  uint16_t x,
  uint16_t y,
  uint16_t width_pixels,
  uint16_t height_pixels,
  uint16_t fColor
);

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




#endif
