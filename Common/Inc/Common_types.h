#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__
#include "stdint.h"

// 创建摇杆全局数据句柄结构体
typedef struct
{
    // 油门轴
    int16_t throttle;
    // 偏航轴
    int16_t yaw;
    // 俯仰轴
    int16_t pitch;
    // 滚转轴
    int16_t roll;
    // 定高
    int16_t hold_height;
    // 关机
    int16_t shutdown;

} Com_joystick_Key_handle_t;

extern Com_joystick_Key_handle_t joystick_key_handle;

// 枚举类型设定 有效无效
typedef enum
{
    eINVALID = 0,
    eVALID,
} VALID_E;

// 设置两个枚举类型 遥控状态  飞机状态
typedef enum
{
    eRC_UNCONNECTED = 0,
    eRC_CONNECTED,
} RC_STATUS_E;

typedef enum
{
    eDONE_IDEL = 0,
    eDONE_NORMAL,
    eDONE_HOLD_HEIGHT,
    eDONE_FAULT,
} PLANE_STATUS_E;

#endif /* __COMMON_TYPES_H__ */
