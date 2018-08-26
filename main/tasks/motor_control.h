#pragma once
#include "freertos/task.h"

void motor_control_init(void);
void motor_control_task(void *pvParameters);