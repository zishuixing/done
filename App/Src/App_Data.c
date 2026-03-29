#include "App_Data.h"

uint8_t rxbufer[32] = {0};
VALID_E App_Data_Receive(Com_joystick_Key_handle_t *p_joystick_key_handle)
{

    // 这里接收到数据  解析数据
    if (!SI24R1_RxPacket(rxbufer))
    {
        // 一次校验
        if (rxbufer[10] == PROTOCOL_ID1 && rxbufer[11] == PROTOCOL_ID2 && rxbufer[12] == PROTOCOL_ID3)
        {
            // 二次校验
            // 校验和校验
            uint32_t checksum = 0;
            for (int i = 0; i < 13; i++)
            {
                checksum += rxbufer[i];
            }
            if (checksum == (rxbufer[13] << 24) | (rxbufer[14] << 16) | (rxbufer[15] << 8) | rxbufer[16])
            {

                // 赋值
                p_joystick_key_handle->pitch = ((int16_t)rxbufer[0] << 8) | rxbufer[1];
                p_joystick_key_handle->roll = ((int16_t)rxbufer[2] << 8) | rxbufer[3];
                p_joystick_key_handle->yaw = ((int16_t)rxbufer[4] << 8) | rxbufer[5];
                p_joystick_key_handle->shutdown = (int16_t)rxbufer[8];
                p_joystick_key_handle->hold_height = (int16_t)rxbufer[9];
                p_joystick_key_handle->throttle = ((int16_t)rxbufer[6] << 8) | rxbufer[7];

                // 打印数据
                printf("pitch: %d, roll: %d, yaw: %d, shutdown: %d, hold_height: %d, throttle: %d\r\n", p_joystick_key_handle->pitch, p_joystick_key_handle->roll, p_joystick_key_handle->yaw, p_joystick_key_handle->shutdown, p_joystick_key_handle->hold_height, p_joystick_key_handle->throttle);

                // 校验通过
                return eVALID;
            }
        }
    }
    return eINVALID;
}
