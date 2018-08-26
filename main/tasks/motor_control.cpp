#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "drivers/esc/pwm.h"

#define SAFETY_PIN           GPIO_NUM_33
#define MOTOR_ESC_PIN        GPIO_NUM_19

extern "C" void motor_control_init(void);
extern "C" void motor_control_task(void *pvParameters);
#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

static float motor_speed = 0.0;
static ESC_PWM *motor_esc;

void motor_control_init(void)
{
    //set up safety pin
    gpio_config_t safety_pin_config;
    safety_pin_config.pin_bit_mask = ((uint64_t)1) << (gpio_num_t)SAFETY_PIN;
    safety_pin_config.mode         = GPIO_MODE_INPUT;
    safety_pin_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    safety_pin_config.pull_up_en   = GPIO_PULLUP_DISABLE;
    safety_pin_config.intr_type    = GPIO_INTR_DISABLE;
    gpio_config(&safety_pin_config);
    gpio_pull_mode_t pull_mode     = GPIO_PULLDOWN_ONLY;
    gpio_set_pull_mode(SAFETY_PIN, pull_mode);
    //Set up motor ESC
    motor_esc = new ESC_PWM(MOTOR_ESC_PIN, STANDARD_PWM);
}

void motor_control_task(void *pvParameters)
{
    while(true)
    {
        uint8_t safety_pin_state = gpio_get_level(SAFETY_PIN);
        uint16_t wanted_speed = motor_speed;
        if(!safety_pin_state)
        {
            printf("pin_state %d\n", safety_pin_state);
            wanted_speed = 0;
        }
        motor_esc->set_speed(wanted_speed);
        delay_ms(1);
    }
}

void motor_control_set_speed(float speed)
{
    motor_speed = speed;
}