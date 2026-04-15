#include "Common_types.h"

// 当前系统唯一一份“最新遥控控制量”全局对象。
// 无线接收任务每次成功收到数据包后，都会更新这里。
Com_joystick_Key_handle_t joystick_key_handle = {0};
