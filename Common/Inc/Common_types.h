#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__
#include "stdint.h"

// 遥控器控制量统一结构体。
// 当前双机协议的核心业务对象就是它：RC 端负责把它打包发送，done 端负责解包还原。
typedef struct
{
    // 油门控制量
    int16_t throttle;

    // 偏航控制量
    int16_t yaw;

    // 俯仰控制量
    int16_t pitch;

    // 横滚控制量
    int16_t roll;

    // 定高功能位
    int16_t hold_height;

    // 关机/停机功能位
    int16_t shutdown;

} Com_joystick_Key_handle_t;

// 全局遥控数据对象：接收任务每次成功解包后，都会把最新控制量写到这里
extern Com_joystick_Key_handle_t joystick_key_handle;

// 表示一次解包是否成功
typedef enum
{
    eINVALID = 0,
    eVALID,
} VALID_E;

// 遥控连接状态
typedef enum
{
    eRC_UNCONNECTED = 0,
    eRC_CONNECTED,
} RC_STATUS_E;

// 飞机工作状态
typedef enum
{
    // 空闲状态
    eDONE_IDEL = 0,

    // 正常状态
    eDONE_NORMAL,

    // 定高状态
    eDONE_HOLD_HEIGHT,

    // 故障状态
    eDONE_FAULT,
} PLANE_STATUS_E;

#endif /* __COMMON_TYPES_H__ */
