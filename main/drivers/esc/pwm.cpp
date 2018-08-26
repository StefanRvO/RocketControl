#include "pwm.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include <cmath>
#include <esp_log.h>

static const char *TAG = "ESC_PWM";


pwm_protocol_frequency_t ESC_PWM::pwm_type;
ledc_timer_config_t ESC_PWM::ledc_timer;
uint8_t ESC_PWM::next_channel_num = LEDC_CHANNEL_0;

ESC_PWM::ESC_PWM(int _pwm_pin, pwm_protocol_frequency_t _pwm_type)
: pwm_pin(_pwm_pin)
{
    if(this->next_channel_num == LEDC_CHANNEL_0)
    {
        this->pwm_type = _pwm_type;
        this->ledc_timer.duty_resolution = LEDC_TIMER_20_BIT;
        this->ledc_timer.freq_hz = this->pwm_type;
        this->ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
        this->ledc_timer.timer_num = LEDC_TIMER_0;
        ledc_timer_config(&this->ledc_timer);
    }
    else if(_pwm_type != this->pwm_type)
    {
        ESP_LOGD(TAG, "Currently only the same PWM frequency is supported on all channels! Using %d for pin %d\n", this->pwm_type, pwm_pin);
    }
    this->ledc_channel.channel    = (ledc_channel_t)this->next_channel_num++;
    this->ledc_channel.duty       = this->get_min_duty();
    this->ledc_channel.gpio_num   = this->pwm_pin;
    this->ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    this->ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel_config(&this->ledc_channel);

}

void ESC_PWM::set_speed(float speed)
{
    uint32_t duty_cycle = this->get_min_duty() + this->get_range() * speed;
    ledc_set_duty(this->ledc_channel.speed_mode, this->ledc_channel.channel, duty_cycle);
    ledc_update_duty(this->ledc_channel.speed_mode, this->ledc_channel.channel);
}


duty_cycle_timings_sec_t ESC_PWM::get_duty_cycles_timings()
{
    //Timing specs from https://www.foxtechfpv.com/product/ESC/BLHeili_S%20User%20Manual.pdf
    duty_cycle_timings_sec_t timings;
    switch(this->pwm_type)
    {
        case STANDARD_PWM:
            timings.min = 0.001;
            timings.max = timings.min * 2;
            break;
        case ONESHOT125:
            timings.min = 0.000125;
            timings.max = timings.min * 2;
            break;
        case ONESHOT42:
            timings.min = 0.0000417;
            timings.max = 0.0000833;
            break;
        case MULTISHOT:
            timings.min = 0.000005;
            timings.max = 0.000025;
        default:
            ESP_LOGD(TAG, "UNSUPPORTED ESC PWM TYPE SELECTED! DEFAULTING TO STANDARD PWM TIMINGS!");
            timings.min = 0.001;
            timings.max = timings.min * 2;
    }
    return timings;
}

//These may be optimised out later if we want. Probably won't matter that much...
uint32_t ESC_PWM::get_min_duty()
{
    double pwm_cycle_time = 1. / this->ledc_timer.freq_hz;
    double min_duty_part = this->get_duty_cycles_timings().min / pwm_cycle_time;
    uint32_t max_duty = (1 << this->ledc_timer.duty_resolution) - 1;
    return max_duty * min_duty_part;
}

uint32_t ESC_PWM::get_max_duty()
{
    double pwm_cycle_time = 1. / this->ledc_timer.freq_hz;
    double max_duty_part = this->get_duty_cycles_timings().max / pwm_cycle_time;
    uint32_t max_duty = (1 << this->ledc_timer.duty_resolution) - 1;
    return max_duty * max_duty_part;
}

uint32_t ESC_PWM::get_range()
{
    return get_max_duty() - get_min_duty();
}