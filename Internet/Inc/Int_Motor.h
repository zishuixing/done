#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__
#include "tim.h"

// 电机句柄：用于描述“一路电机输出”需要的全部硬件信息
typedef struct
{
    // 电机对应的定时器
    TIM_HandleTypeDef *htim;

    // 电机对应的 PWM 通道
    uint32_t channel;

    // 当前目标速度值，本质上会被写进 PWM 比较寄存器
    uint16_t speed;
} Motor_Handle_t;

// 4 路电机全局句柄
extern Motor_Handle_t motor_left0_top_handle;
extern Motor_Handle_t motor_right0_top_handle;
extern Motor_Handle_t motor_left1_bottom_handle;
extern Motor_Handle_t motor_right1_bottom_handle;

// 打开所有电机 PWM 输出通道
void Int_Motor_Init(void);

// 把指定电机句柄中的 speed 写入实际 PWM 输出
void Int_Motor_SetSpeed(Motor_Handle_t *motor_handle);

#endif /* __INT_MOTOR_H__ */
