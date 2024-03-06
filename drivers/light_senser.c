#include "light_sensor.h"

uint16_t light_sensor(void)
{
    uint16_t brightness_val;
    int newdata_status;
    uint8_t ch_0_read_val[2] = {0, 0};
    uint8_t ch_1_read_val[2] = {0, 0};
    
    while(1){
        //printf("aaaaaaaaaaa\n");
        io_expander_set_ctrl_port(CTRL_CMD);
        // printf("aaaaaaaaaaa\n");
        io_expander_set_measure(MEASURE_T);
        // printf("aaaaaaaaaaa\n");
        ch_0_read_val[0] = io_expander_read_reg(ALS_CH0_0_ADDR);
        ch_0_read_val[1] = io_expander_read_reg(ALS_CH0_1_ADDR);
        ch_1_read_val[0] = io_expander_read_reg(ALS_CH1_0_ADDR);
        ch_1_read_val[1] = io_expander_read_reg(ALS_CH1_1_ADDR);

        brightness_val = (ch_0_read_val[1] << 8) | ch_0_read_val[0];
        newdata_status = io_expander_read_reg(ALS_STATUS) & ALS_STATUS_BIT;
        if (!newdata_status)
        {
           // printf("aaaaaaaaaaaaaaaaaaaa\n");
            return brightness_val;
        }
    }
    
}