#include <stdio.h>

#include "led.h"
#include "button.h"
#include "watering_fsm.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define W_BTN_ON_PIN   14
#define W_BTN_OFF_PIN  18
#define W_BTN_MANU_PIN 19
#define W_BTN_AUTO_PIN 21
#define W_BTN_OP_LICE  22
#define W_BTN_RESET    23

button_ctr_t w_btn_on;
button_ctr_t w_btn_off;
button_ctr_t w_btn_manu;
button_ctr_t w_btn_auto;
button_ctr_t w_btn_op_lice;
button_ctr_t w_btn_reset;

w_mode_ctrl_t w_ctrl_str;

void app_main(void)
{
    init_button(W_BTN_ON_PIN,   &w_btn_on);
    init_button(W_BTN_OFF_PIN,  &w_btn_off);
    init_button(W_BTN_MANU_PIN, &w_btn_manu);
    init_button(W_BTN_AUTO_PIN, &w_btn_auto);
    init_button(W_BTN_OP_LICE,  &w_btn_op_lice);
    init_button(W_BTN_RESET,    &w_btn_reset);

    while(1)
    {
        read_button(&w_btn_on);
        read_button(&w_btn_off);
        read_button(&w_btn_manu);
        read_button(&w_btn_auto);
        read_button(&w_btn_op_lice);
        read_button(&w_btn_reset);

        w_ctrl_str.ctrl_bits_t.w_op_lice    = w_btn_op_lice.pressed;
        w_ctrl_str.ctrl_bits_t.w_mode_reset = w_btn_reset.pressed;

        update_w_ctrl_bits(&w_ctrl_str,
                           &w_btn_manu,
                           &w_btn_auto,
                           &w_btn_on,
                           &w_btn_off);
        
        printf("OP Lice: %d\n", w_ctrl_str.ctrl_bits_t.w_op_lice);
        printf("Reset: %d\n", w_ctrl_str.ctrl_bits_t.w_mode_reset);
        
        update_states(&w_ctrl_str);
    }
}
