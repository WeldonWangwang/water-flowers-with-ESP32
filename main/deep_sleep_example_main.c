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
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_adc_cal.h"

#define GPIO_WAKEUP       25    // GPIO25 connect to DO and use EXIT1 
#define GPIO_MOTOR        4     // The GPIO to control motor, we use LED to replace motor
#define GPIO_RELAY        5     // The GPIO to control relay
#define GPIO_INPUT_PIN_SEL     (1<<GPIO_WAKEUP)
#define GPIO_OUTPUT_PIN_SEL    ((1<<GPIO_MOTOR) | (1<<GPIO_RELAY))

#define V_REF   1100
#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_6)      //GPIO 34
static RTC_DATA_ATTR struct timeval sleep_enter_time;


/* To enable TIMER mode, uncomment the following line:
 */
#define TIMER_MODE

void gpio_init(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;   
    io_conf.mode = GPIO_MODE_INPUT;             
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;  
    io_conf.pull_down_en = 0;                    
    io_conf.pull_up_en = 0;                      
    gpio_config(&io_conf);                       
    gpio_set_level(25, 0);
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;   
    io_conf.mode = GPIO_MODE_OUTPUT;             
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;  
    io_conf.pull_down_en = 0;                    
    io_conf.pull_up_en = 0;                 
    gpio_config(&io_conf);                       
}

/* The water time should change with the change of soil moisture, so if we use the TIMER_MODE, we should 
 * change the water time by the value read from the sensor
 */
void water(void)
{
    printf("**********************START WATERING**********************\n");
    gpio_set_level(4, 0);
    vTaskDelay(3000 / portTICK_RATE_MS);
    gpio_set_level(4, 1);
    printf("**********************END WATERING**********************\n");
}
/* We use a low active  relay to control the sensor
 *  1: ON;
 *  0: OFF;
 */
void sensor(bool state)
{
    state = !state;
    gpio_set_level(GPIO_RELAY, state);
}

uint32_t read_sensor(void)
{
    esp_adc_cal_characteristics_t characteristics;
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_11db);
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_0db, ADC_WIDTH_12Bit, &characteristics);
    uint32_t voltage;

    voltage = adc1_to_voltage(ADC1_TEST_CHANNEL, &characteristics);
    printf("%d mV\n", voltage);
    vTaskDelay(100 / portTICK_RATE_MS);
    return voltage;
}

void app_main()
{
#ifndef TIMER_MODE
    const int ext_wakeup_pin = 25;
    const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin;

    gpio_init();

    uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
    if (wakeup_pin_mask != 0) {
        int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
        printf("Wake up from GPIO %d\n", pin);
        water();
    }
    esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
#else
    uint32_t val;
    struct timeval now;
    gpio_init();
        gpio_set_level(4, 1);     
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

    printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
    const int wakeup_time_sec = 5;   //if set this as 43200,it is 12 hours
    printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);

    sensor(1);
    //vTaskDelay(10000 / portTICK_RATE_MS);
    for (int i = 0;i<2;i++) {
        val = read_sensor();
    }
    if (val >= 500)
        water();

    sensor(0);
#endif

    printf("deep_sleep_start\n");
    esp_deep_sleep_start();
}