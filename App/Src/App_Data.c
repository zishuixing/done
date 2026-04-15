#include "App_Data.h"

// 无线模块每次固定接收 32 字节，这里用来暂存最近一次收到的原始数据包
uint8_t rxbufer[32] = {0};

VALID_E App_Data_Receive(Com_joystick_Key_handle_t *p_joystick_key_handle)
{
    // 先尝试从 SI24R1 读取一包数据。
    // 当前驱动约定：返回 0 表示收到了数据，返回 1 表示当前没有新数据。
    if (!SI24R1_RxPacket(rxbufer))
    {
        // 第一层校验：检查协议 ID，防止把错误数据误当成本项目协议包
        if (rxbufer[10] == PROTOCOL_ID1 && rxbufer[11] == PROTOCOL_ID2 && rxbufer[12] == PROTOCOL_ID3)
        {
            // 第二层校验：对前 13 字节做求和校验。
            // 这里必须和 RC 遥控器端的打包逻辑完全一致。
            uint32_t checksum = 0;
            for (int i = 0; i < 13; i++)
            {
                checksum += rxbufer[i];
            }

            if (checksum == (rxbufer[13] << 24) | (rxbufer[14] << 16) | (rxbufer[15] << 8) | rxbufer[16])
            {
                // 校验通过后，按固定协议顺序把字节流还原成业务结构体
                p_joystick_key_handle->pitch = ((int16_t)rxbufer[0] << 8) | rxbufer[1];
                p_joystick_key_handle->roll = ((int16_t)rxbufer[2] << 8) | rxbufer[3];
                p_joystick_key_handle->yaw = ((int16_t)rxbufer[4] << 8) | rxbufer[5];
                p_joystick_key_handle->shutdown = (int16_t)rxbufer[8];
                p_joystick_key_handle->hold_height = (int16_t)rxbufer[9];
                p_joystick_key_handle->throttle = ((int16_t)rxbufer[6] << 8) | rxbufer[7];

                // 打印当前收到的控制量，方便调试协议是否正常
                printf("pitch: %d, roll: %d, yaw: %d, shutdown: %d, hold_height: %d, throttle: %d\r\n",
                       p_joystick_key_handle->pitch,
                       p_joystick_key_handle->roll,
                       p_joystick_key_handle->yaw,
                       p_joystick_key_handle->shutdown,
                       p_joystick_key_handle->hold_height,
                       p_joystick_key_handle->throttle);

                // 表示本次收包、验包、解包全部成功
                return eVALID;
            }
        }
    }

    // 没有收到包，或者协议 ID 不匹配，或者校验失败
    return eINVALID;
}
