#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "watering_fsm.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static states_t watering_current_state, watering_previous_state;
watering_current_state = BOOTING;

void update_w_ctrl_bits(w_mode_ctrl_t *p_me,
                        button_ctr_t  *p_w_mode_manu,
                        button_ctr_t  *p_w_mode_auto,
                        button_ctr_t  *p_w_button_on,
                        button_ctr_t  *p_w_button_off,
                    )
{
    p_me->ctrl_bits_t.w_mode_manu  = p_w_mode_manu-> pressed;
    printf("Mode Manu: %d", p_me->ctrl_bits_t.w_mode_manu);
    p_me->ctrl_bits_t.w_mode_auto  = p_w_mode_auto-> pressed;
    printf("Mode Auto: %d", p_me->ctrl_bits_t.w_mode_manu);
    p_me->ctrl_bits_t.w_button_on  = p_w_button_on-> pressed;
    printf("Mode Button ON: %d", p_me->ctrl_bits_t.w_button_on);
    p_me->ctrl_bits_t.w_button_off = p_w_button_off->pressed;
    printf("Mode Button OFF: %d", p_me->ctrl_bits_t.w_button_off);
}

void update_states(w_mode_ctrl_t *p_w_ctrl)
{
    switch (watering_current_state)
    {
        case BOOTING:
            printf("Entrei no BOOTING\n");
            // T1
            vTaskDelay(5000/portTICK_PERIOD_MS);
            
            watering_previous_state = watering_current_state;
            watering_current_state  = IDLE;

            printf("Sai do BOOTING e fui pro IDLE\n");
            break;
        
        case IDLE:
            printf("Estou no IDLE\n");
            // T2
            if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
            && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
            && p_w_ctrl->ctrl_bits_t.w_mode_auto == 0)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = MANUAL;

                printf("Sai do IDLE e fui pro MANUAL\n");
                break;
            }
            
            // T3
            else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
                 && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = AUTO;

                printf("Sai do IDLE e fui pro AUTO\n");
                break;
            }

        case MANUAL:
            printf("Estou no MANUAL\n");
            // T4
            if(p_w_ctrl->ctrl_bits_t.w_op_lice == 0)
            {
                printf("Sai do MANUAL e fui pro IDLE\n");
                break;
            }
            
            // T6
            else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = FAIL;

                printf("Sai do MANUAL e fui pro FAIL");
                break;
            }
            
            // T7
            else if(p_w_ctrl->ctrl_bits_t.w_op_lice    == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_manu  == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_auto  == 0
                 && p_w_ctrl->ctrl_bits_t.w_button_on  == 1
                 && p_w_ctrl->ctrl_bits_t.w_button_off == 0)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = RUNNING;

                printf("Sai do MANUAL e fui pro RUNNING");
                break;
            }

            // T8
            else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
                 && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = AUTO;

                printf("Sai do MANUAL e fui pro AUTO");
                break;
            }
    
    case AUTO:
        printf("Entrei no AUTO\n");
        // T5
        if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_auto == 0)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = MANUAL;

            printf("Sai do AUTO e fui pro MANUAL\n");
            break;
        }
        // T9
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
             && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1
             && p_w_ctrl->ctrl_bits_t.w_dry_flag  == 1
             &&p_w_ctrl->ctrl_bits_t.w_wet_flag   == 0)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = RUNNING;

            printf("Sai do AUTO e fui pro RUNNING\n");
            break;
        }

        // T10
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = FAIL;

            printf("Sai do AUTO e fui pro FAIL");
            break;
        }

        // T15
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice == 0)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = IDLE;

            printf("Sai do AUTO e fui pro IDLE\n");
            break;
        }
    
    case RUNNING:
        printf("Entrei no RUNNING\n");

        // T11
        if(p_w_ctrl->ctrl_bits_t.w_op_lice    == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_manu  == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_auto  == 0
        && p_w_ctrl->ctrl_bits_t.w_button_off == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = MANUAL;

            printf("Sai do RUNNING e fui pro MANUAL\n");
            break;
        }
        
        // T12
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
             && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1
             && p_w_ctrl->ctrl_bits_t.w_dry_flag  == 0
             && p_w_ctrl->ctrl_bits_t.w_wet_flag  == 1)

        {
            watering_previous_state = watering_current_state;
            watering_current_state  = AUTO;

            printf("Sai do RUNNING e fui pro AUTO\n");
            break;
        }

        // T13
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = FAIL;

            printf("Sai do RUNNING e fui pro FAIL\n");
            break;
        }  
        
        // T16
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice == 0)
        {
            watering_previous_state = watering_current_state;
            watering_current_state = IDLE;

            printf("Sai do RUNNING e fui pro IDLE\n");
        }
    
    case FAIL:
        printf("Entrei no FAIL\n");

        // T14
        if(p_w_ctrl->ctrl_bits_t.w_op_lice == 0
        && p_w_ctrl->ctrl_bits_t.w_mode_reset == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state = IDLE;

            printf("Sai do FAIL e fui pro IDLE\n");
        }
    
    default:
        break;
    }
}

