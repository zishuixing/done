#ifndef __APP_DATA_H__
#define __APP_DATA_H__
#include "Int_SI4R1.h"
#include "Common_types.h"

// 设置发送2.4g数据协议 防止碰撞干扰
#define PROTOCOL_ID1 0x01
#define PROTOCOL_ID2 0x08
#define PROTOCOL_ID3 0x04
// 数据接收
VALID_E App_Data_Receive(Com_joystick_Key_handle_t *p_joystick_key_handle);

#endif /* __APP_DATA_H__ */
