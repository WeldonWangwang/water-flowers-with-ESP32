#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_IO_0       25
#define GPIO_OUTPUT_PIN_SEL    ((1<<GPIO_OUTPUT_IO_0))// | (1<<GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0        4
#define GPIO_INPUT_IO_1        5
#define GPIO_INPUT_PIN_SEL     ((1<<GPIO_INPUT_IO_0) | (1<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT  0



void app_main()
{
//    gpio_init();
    const int ext_wakeup_pin_2 = 25;
    const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;   

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;   // disable interrupt
    io_conf.mode = GPIO_MODE_INPUT;             // set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;  // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = 0;                    // disable pull-down mode
    io_conf.pull_up_en = 0;                      // disable pull-up mode
    gpio_config(&io_conf);                       // configure GPIO with the given settings

    gpio_set_level(25, 0);



    uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
    if (wakeup_pin_mask != 0) {
        int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
        printf("Wake up from GPIO %d\n", pin);
    } 

    esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
    printf("deep_sleep_start\n");
    esp_deep_sleep_start();
}