/**
 * @file systick.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "systick.h"

/**
 * @brief
 *  Initilaizes the SysTick Timer for a configurable interval of time.
 * @param ticks
 *  The Number clock cycles
 */
void systick_init(uint32_t ticks)
{
    if (ticks > 0xFFFFFF)
    {
        return false;
    }
    SysTick->CTRL = 0;
    SysTick->VAL = 0;
    SysTick->LOAD = ticks - 1;
    SysTick->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_ENABLE;
    return true;
}

bool systick_expired(void)
{
    if (SysTick->CTRL & SYSTICK_CTRL_COUNTFLAG)
    {
        return true;
    }
    return false;
}