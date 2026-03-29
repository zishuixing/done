#include "Int_Motor.h"

// 初始化4个电机句柄并赋值
Motor_Handle_t motor_left0_top_handle = {.htim = &htim3, .channel = TIM_CHANNEL_1, .speed = 0};
Motor_Handle_t motor_right0_top_handle = {.htim = &htim2, .channel = TIM_CHANNEL_2, .speed = 0};
Motor_Handle_t motor_left1_bottom_handle = {.htim = &htim4, .channel = TIM_CHANNEL_4, .speed = 0};
Motor_Handle_t motor_right1_bottom_handle = {.htim = &htim1, .channel = TIM_CHANNEL_3, .speed = 0};

void Int_Motor_Init(void)
{
    // 开启tim的pwm输出start
    HAL_TIM_PWM_Start(motor_left0_top_handle.htim, motor_left0_top_handle.channel);
    HAL_TIM_PWM_Start(motor_left1_bottom_handle.htim, motor_left1_bottom_handle.channel);
    HAL_TIM_PWM_Start(motor_right0_top_handle.htim, motor_right0_top_handle.channel);
    HAL_TIM_PWM_Start(motor_right1_bottom_handle.htim, motor_right1_bottom_handle.channel);
}

// 设置电机速度
void Int_Motor_SetSpeed(Motor_Handle_t *motor_handle)
{
    __HAL_TIM_SetCompare(motor_handle->htim, motor_handle->channel, motor_handle->speed);
}
