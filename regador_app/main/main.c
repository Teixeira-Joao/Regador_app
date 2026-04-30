#include <stdio.h>

#include "button.h"
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BTN_1_PIN 23
#define LED_1_PIN 2

led_ctr_t led_1;
button_ctr_t button_1;

void app_main(void)
{
    init_led(LED_1_PIN, &led_1);
    init_button(BTN_1_PIN, &button_1);

    while(1)
    {
        read_button(&button_1);

        if(button_1.pressed)
        {
            turn_led_on(&led_1);
            printf("Button status %d\n", button_1.pressed);
            printf("Led status: %d\n", led_1.led_status);
            printf("Estou rodando no button pressed\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        else
        {
            turn_led_off(&led_1);
            printf("Button status: %d\n", button_1.pressed);
            printf("Led status: %d\n", led_1.led_status);
            printf("Estou rodando no button released\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
    }
}
