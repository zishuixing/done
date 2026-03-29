#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__
#include "tim.h"

// 设置motor句柄结构体
typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    uint16_t speed;
} Motor_Handle_t;

// extern 全局变量
//  初始化4个电机句柄并赋值
extern Motor_Handle_t motor_left0_top_handle;
extern Motor_Handle_t motor_right0_top_handle;
extern Motor_Handle_t motor_left1_bottom_handle;
extern Motor_Handle_t motor_right1_bottom_handle;

void Int_Motor_Init(void);
void Int_Motor_SetSpeed(Motor_Handle_t *motor_handle);

#endif /* __INT_MOTOR_H__ */
