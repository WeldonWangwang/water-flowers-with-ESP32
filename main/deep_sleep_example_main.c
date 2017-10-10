#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"

#define GPIO_OUTPUT_IO_0       25
#define GPIO_OUTPUT_PIN_SEL    ((1<<GPIO_OUTPUT_IO_0))// | (1<<GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0        4
#define GPIO_INPUT_IO_1        5
#define GPIO_INPUT_PIN_SEL     ((1<<GPIO_INPUT_IO_0) | (1<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT  0

static RTC_DATA_ATTR struct timeval sleep_enter_time;

#define ULP_MODE

void wakeup_gpio_init(void)
{
    const int ext_wakeup_pin_1 = 25;
    const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;   // disable interrupt
    io_conf.mode = GPIO_MODE_INPUT;             // set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;  // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = 0;                    // disable pull-down mode
    io_conf.pull_up_en = 0;                      // disable pull-up mode
    gpio_config(&io_conf);                       // configure GPIO with the given settings
    gpio_set_level(25, 0);
}

void motor_control(void)
{
    ;
}

void app_main()
{   
#ifndef ULP_MODE
    wakeup_gpio_init();

    uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
    if (wakeup_pin_mask != 0) {
        int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
        printf("Wake up from GPIO %d\n", pin);
    } 
    esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
#endif

#ifdef ULP_MODE
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;
    
    printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
    const int wakeup_time_sec = 5;   //43200 is 12 hours
    printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);
#endif
    motor_control();
    

    printf("deep_sleep_start\n");
    esp_deep_sleep_start();
}