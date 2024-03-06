#ifndef __TASK_LCD_H__
#define __TASK_LCD_H_

#include "splatoon.h"

extern struct Player cur_player;
void init_lcd_task(void);
extern int playtime;
#define game_time 7
extern project_state state;
#endif