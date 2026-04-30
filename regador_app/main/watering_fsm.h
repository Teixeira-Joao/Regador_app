#ifndef WATERING_FSM_H
#define WATERING_FSM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "components/led/include/led.h"
#include "components/button/include/button.h"

typedef enum states
{
    BOOTING = 1,
    IDLE,
    MANUAL,
    AUTO,
    RUNNING,
    FAIL
}states_t;

typedef union
{
    typedef struct 
    {
        // Updated by GPIO inputs
        uint16_t w_mode_manu  : 1;
        uint16_t w_mode_auto  : 1;
        uint16_t w_button_on  : 1;
        uint16_t w_button_off : 1;
        uint16_t w_dry_flag   : 1;
        uint16_t w_wet_flag   : 1;

        // Updated by Supervisory FSM
        uint16_t w_op_lice    : 1;
        uint16_t w_mode_reset : 1;

        // Output bits
        uint16_t w_output_1_on : 1;
        uint16_t w_output_2_on : 1;
        uint16_t w_output_3_on : 1;
        uint16_t w_output_4_on : 1;
        uint16_t free_bits     : 4;
    }ctrl_bits_t;
    uint16_t ctrl_bytes;      
}w_mode_ctrl_t;

void update_w_ctrl_bits(w_mode_ctrl_t *p_me,
                        button_ctr_t  *p_w_mode_manu,
                        button_ctr_t  *p_w_mode_auto,
                        button_ctr_t  *p_w_button_on,
                        button_ctr_t  *p_w_button_off,
                    );

void update_states(w_mode_ctrl_t *p_w_ctrl);

#endif