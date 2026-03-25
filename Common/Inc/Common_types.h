#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

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
