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

THR_Status_e thr_status = eFREE;
uint32_t Max_first_time = 0;
uint32_t Min_first_time = 0;
THR_Status_e App_Data_Unlock(Com_joystick_Key_handle_t *p_handle_data)
{
    uint16_t throttle = p_handle_data->throttle;
    switch (thr_status)
    {
    case eFREE:
        printf("FREE\r\n");
        /* code */
        if (throttle >= 900)
        {
            thr_status = eMAX;
            Max_first_time = xTaskGetTickCount();
        }
        break;
    case eMAX:
        printf("MAX\r\n");
        if (throttle < 900)
        {
            if (xTaskGetTickCount() - Max_first_time >= 1000)
            {
                /* code */
                thr_status = eLEAVEMAX;
            }
            else
            {
                thr_status = eFREE;
            }
        }
        break;
    case eLEAVEMAX:
        printf("LEAVEMAX\r\n");
        if (throttle <= 100)
        {
            thr_status = eMIN;
            Min_first_time = xTaskGetTickCount();
        }
        break;

    case eMIN:
        printf("MIN\r\n");
        if (throttle > 100)
        {
            if (xTaskGetTickCount() - Min_first_time <= 1000)
            {
                thr_status = eFREE;
            }
        }
        if (throttle <= 100)
        {
            if (xTaskGetTickCount() - Min_first_time >= 1000)
            {
                thr_status = eUNLOCK;
            }
        }

        break;

    case eUNLOCK:
        printf("unlock\r\n");
        return thr_status;
        break;
    }

    return thr_status;
}

void App_Data_ProcessDoneState(Com_joystick_Key_handle_t *p_handle_data, PLANE_STATUS_E *plane_status, RC_STATUS_E *rc_status)
{

    switch (*plane_status)
    {
    case eDONE_IDEL:
        if (App_Data_Unlock(p_handle_data) == eUNLOCK)
        {
            *plane_status = eDONE_NORMAL;
        }
        break;
    case eDONE_NORMAL:
        if (*rc_status == eRC_UNCONNECTED)
        {
            *plane_status = eDONE_FAULT;
        }
        else
        {
            if (p_handle_data->hold_height == 1)
            {
                *plane_status = eDONE_HOLD_HEIGHT;
            }
        }
        break;

    case eDONE_HOLD_HEIGHT:
        if (*rc_status == eRC_UNCONNECTED)
        {
            *plane_status = eDONE_FAULT;
        }
        else
        {
            if (p_handle_data->hold_height == 0)
            {
                *plane_status = eDONE_NORMAL;
            }
        }
        break;

    case eDONE_FAULT:
        if (*rc_status == eRC_CONNECTED)
        {
            *plane_status = eDONE_NORMAL;
        }
        break;
    }
}
