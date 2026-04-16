#ifndef __APP_DATA_H__
#define __APP_DATA_H__

#include "Int_SI4R1.h"
#include "Common_types.h"

// 应用层协议标识。
// 这 3 个字节用于告诉接收端：当前这包数据是本项目定义的遥控协议。
#define PROTOCOL_ID1 0x01
#define PROTOCOL_ID2 0x08
#define PROTOCOL_ID3 0x04

// 从 SI24R1 接收缓冲区中取包，并按本项目协议解析成业务结构体
VALID_E App_Data_Receive(Com_joystick_Key_handle_t *p_joystick_key_handle);

// 解锁遥控器。
THR_Status_e App_Data_Unlock(Com_joystick_Key_handle_t *p_handle_data);

void App_Data_ProcessDoneState(Com_joystick_Key_handle_t *p_handle_data, PLANE_STATUS_E *plane_status, RC_STATUS_E *rc_status); 

#endif /* __APP_DATA_H__ */
