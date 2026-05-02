#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "watering_fsm.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

states_t watering_current_state = BOOTING;
states_t watering_previous_state;

void update_w_ctrl_bits(w_mode_ctrl_t *p_me,
                        button_ctr_t  *p_w_mode_manu,
                        button_ctr_t  *p_w_mode_auto,
                        button_ctr_t  *p_w_button_on,
                        button_ctr_t  *p_w_button_off)
{
    p_me->ctrl_bits_t.w_mode_manu  = p_w_mode_manu-> pressed;
    printf("Mode Manu: %d\n", p_me->ctrl_bits_t.w_mode_manu);
    p_me->ctrl_bits_t.w_mode_auto  = p_w_mode_auto-> pressed;
    printf("Mode Auto: %d\n", p_me->ctrl_bits_t.w_mode_auto);
    p_me->ctrl_bits_t.w_button_on  = p_w_button_on-> pressed;
    printf("Mode Button ON: %d\n", p_me->ctrl_bits_t.w_button_on);
    p_me->ctrl_bits_t.w_button_off = p_w_button_off->pressed;
    printf("Mode Button OFF: %d\n", p_me->ctrl_bits_t.w_button_off);
    vTaskDelay(5000/portTICK_PERIOD_MS);
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
            vTaskDelay(1000/portTICK_PERIOD_MS);
            break;
        
        case IDLE:
            printf("Estou no IDLE\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            // T2
            if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
            && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
            && p_w_ctrl->ctrl_bits_t.w_mode_auto == 0)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = MANUAL;

                printf("Sai do IDLE e fui pro MANUAL\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);
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
                vTaskDelay(1000/portTICK_PERIOD_MS);
                break;
            }
            break;

        case MANUAL:
            printf("Estou no MANUAL\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            // T4
            if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 0
            ||(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
            && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
            && p_w_ctrl->ctrl_bits_t.w_mode_auto == 0))
            {
                watering_previous_state = watering_current_state;
                watering_current_state = IDLE;
                printf("Sai do MANUAL e fui pro IDLE\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);
                break;
            }
            
            // T6
            else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = FAIL;

                printf("Sai do MANUAL e fui pro FAIL\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);
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

                printf("Sai do MANUAL e fui pro RUNNING\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);
                break;
            }

            // T8
            else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
                 && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
                 && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
            {
                watering_previous_state = watering_current_state;
                watering_current_state  = AUTO;

                printf("Sai do MANUAL e fui pro AUTO\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);
                break;
            }
            break;
    
    case AUTO:
        printf("Entrei no AUTO\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        // T5
        if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_auto == 0)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = MANUAL;

            printf("Sai do AUTO e fui pro MANUAL\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
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
            vTaskDelay(1000/portTICK_PERIOD_MS);
            break;
        }

        // T10
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_manu == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_auto == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = FAIL;

            printf("Sai do AUTO e fui pro FAIL\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            break;
        }

        // T15
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice   == 0
             ||(p_w_ctrl->ctrl_bits_t.w_op_lice   == 1
             && p_w_ctrl->ctrl_bits_t.w_mode_manu == 0
             && p_w_ctrl->ctrl_bits_t.w_mode_auto == 0))
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = IDLE;

            printf("Sai do AUTO e fui pro IDLE\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            break;
        }
        break;
    
    case RUNNING:
        printf("Entrei no RUNNING\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);

        // T11
        if(p_w_ctrl->ctrl_bits_t.w_op_lice    == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_manu  == 1
        && p_w_ctrl->ctrl_bits_t.w_mode_auto  == 0
        && p_w_ctrl->ctrl_bits_t.w_button_off == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state  = MANUAL;

            printf("Sai do RUNNING e fui pro MANUAL\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
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
            vTaskDelay(1000/portTICK_PERIOD_MS);
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
            vTaskDelay(1000/portTICK_PERIOD_MS);
            break;
        }  
        
        // T16
        else if(p_w_ctrl->ctrl_bits_t.w_op_lice == 0)
        {
            watering_previous_state = watering_current_state;
            watering_current_state = IDLE;

            printf("Sai do RUNNING e fui pro IDLE\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        break;
    
    case FAIL:
        printf("Entrei no FAIL\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);

        // T14
        if(p_w_ctrl->ctrl_bits_t.w_op_lice == 0
        && p_w_ctrl->ctrl_bits_t.w_mode_reset == 1)
        {
            watering_previous_state = watering_current_state;
            watering_current_state = IDLE;

            printf("Sai do FAIL e fui pro IDLE\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
    }
}

