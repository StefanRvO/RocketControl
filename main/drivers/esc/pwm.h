#pragma once
#include "driver/ledc.h"

void ESC_PWM_set_speed(float speed);

enum pwm_protocol_frequency_t
{
    STANDARD_PWM = 50,
    ONESHOT125   = 2000,
    ONESHOT42    = 10000,
    MULTISHOT    = 30000,
};

struct duty_cycle_timings_sec_t
{
    float min;
    float max;
};

class ESC_PWM
{
    private:
        uint32_t get_max_duty();
        uint32_t get_min_duty();
        uint32_t get_range();
        static pwm_protocol_frequency_t pwm_type;
        static ledc_timer_config_t ledc_timer;
        static uint8_t next_channel_num;
        int pwm_pin;
        ledc_channel_config_t ledc_channel;
        duty_cycle_timings_sec_t get_duty_cycles_timings();
    public:
        void set_speed(float speed);
        ESC_PWM(int pwm_pin, pwm_protocol_frequency_t pwm_type);
};