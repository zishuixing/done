#include "Int_Motor.h"

// 4 路电机句柄。
// 这里把“哪一个定时器、哪一个通道、当前速度值”统一封装起来，后续控制层只需要操作这些句柄即可。
Motor_Handle_t motor_left0_top_handle = {.htim = &htim3, .channel = TIM_CHANNEL_1, .speed = 0};
Motor_Handle_t motor_right0_top_handle = {.htim = &htim2, .channel = TIM_CHANNEL_2, .speed = 0};
Motor_Handle_t motor_left1_bottom_handle = {.htim = &htim4, .channel = TIM_CHANNEL_4, .speed = 0};
Motor_Handle_t motor_right1_bottom_handle = {.htim = &htim1, .channel = TIM_CHANNEL_3, .speed = 0};

void Int_Motor_Init(void)
{
    // 打开 4 路 PWM 输出。
    // 这里只是开启 PWM 通道，本身并不会自动给出某个速度值，真正占空比还要靠 SetSpeed 写入。
    HAL_TIM_PWM_Start(motor_left0_top_handle.htim, motor_left0_top_handle.channel);
    HAL_TIM_PWM_Start(motor_left1_bottom_handle.htim, motor_left1_bottom_handle.channel);
    HAL_TIM_PWM_Start(motor_right0_top_handle.htim, motor_right0_top_handle.channel);
    HAL_TIM_PWM_Start(motor_right1_bottom_handle.htim, motor_right1_bottom_handle.channel);
}

void Int_Motor_SetSpeed(Motor_Handle_t *motor_handle)
{
    // 把 speed 写到对应定时器通道的比较寄存器中。
    // 从业务角度看，这一步就是把“目标速度值”真正变成 PWM 输出。
    __HAL_TIM_SetCompare(motor_handle->htim, motor_handle->channel, motor_handle->speed);
}
