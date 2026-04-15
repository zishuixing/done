#include "Int_SI4R1.h"

// 当前项目固定使用的一组无线地址。
// RC 遥控器端和 done 端必须保持完全一致，否则两边无法建立链路。
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01};

// 最底层 SPI 读写函数：发送一个字节，同时读回一个字节
static uint8_t SPI_RW(uint8_t byte)
{
    uint8_t rx_data;

    HAL_SPI_TransmitReceive(&hspi1, &byte, &rx_data, 1, HAL_MAX_DELAY);

    return rx_data;
}

void Int_SI4R1_Init(void)
{
    // 先给无线模块一个确定的初始电平状态
    SPI_NSS_HIGH;
    SI4R1_CE_LOW;
    // IRQ/状态脚这里作为输入使用，不需要主动输出
}

void Int_SI24R1_TX_Mode(void)
{
    // 拉低 CE，进入配置阶段
    SI4R1_CE_LOW;

    // 写发送地址
    SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);

    // 通道 0 的接收地址也写成同样的地址，用于接收自动应答
    SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);

    // 只开启 pipe0 的自动应答
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);

    // 只开启 pipe0 接收地址
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);

    // 自动重发参数配置
    SI24R1_Write_Reg(SI24R1_WRITE_REG + SETUP_RETR, 0x0a);

    // 选择无线通道 40。双机必须一致。
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, 40);

    // 射频配置：当前项目选择 2Mbps 和较大发射功率
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x0f);

    // 发送模式配置：CRC 使能、上电、工作在发送模式
    SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0e);

    // 注意：这里只是完成配置，并没有立即发包。
    // 真正发送时还需要把数据写进 TX FIFO 并拉高 CE。
}

uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;

    SPI_NSS_LOW;
    status = SPI_RW(reg);
    SPI_RW(value);
    SPI_NSS_HIGH;

    return status;
}

uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status, byte_ctr;

    SPI_NSS_LOW;

    status = SPI_RW(reg);
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
    {
        SPI_RW(*pBuf++);
    }

    SPI_NSS_HIGH;

    return status;
}

uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
    uint8_t state;

    // 拉低 CE，进入装载数据阶段
    SI4R1_CE_LOW;

    // 把 32 字节数据写入 TX FIFO
    SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);

    // 拉高 CE，触发本次发射
    SI4R1_CE_HIGH;

    // 等待 IRQ/状态脚变化，表示发送结束
    while (HAL_GPIO_ReadPin(SI_IRO_GPIO_Port, SI_IRO_Pin) == 1)
    {
    }

    // 读取发送结果状态
    state = SI24R1_Read_Reg(STATUS);

    // 清除发送相关中断标志
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state);

    if (state & MAX_RT)
    {
        // 达到最大重发次数后，清掉 TX FIFO，避免后续一直卡着旧数据
        SI24R1_Write_Reg(FLUSH_TX, 0xff);
        return MAX_RT;
    }

    if (state & TX_DS)
    {
        return TX_DS;
    }

    return 0XFF;
}

uint8_t SI24R1_Read_Reg(uint8_t reg)
{
    uint8_t value;

    SPI_NSS_LOW;
    SPI_RW(reg);
    value = SPI_RW(0);
    SPI_NSS_HIGH;

    return value;
}

void SI24R1_RX_Mode(void)
{
    // 拉低 CE，先进入配置阶段
    SI4R1_CE_LOW;

    // pipe0 接收地址和发送端地址保持一致
    SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);

    // 开启 pipe0 自动应答
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);

    // 开启 pipe0 接收
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);

    // 无线通道必须和发送端一致
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, 40);

    // 接收有效载荷长度固定为 32 字节
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);

    // 射频速率和功率配置
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x0f);

    // 接收模式配置：CRC 使能、上电、工作在接收模式
    SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0f);

    // 清除所有状态标志
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, 0xff);

    // 你原注释里提到过这里是为了规避硬件/状态残留问题。
    // 进入 RX 模式前先清 RX FIFO，避免旧数据影响当前接收。
    SI24R1_Write_Reg(FLUSH_RX, 0xff);

    // 拉高 CE，正式开始接收
    SI4R1_CE_HIGH;
}

uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
    uint8_t state;

    // 先读状态寄存器，看看是否有新包到达
    state = SI24R1_Read_Reg(STATUS);

    // 清除 RX 完成标志
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state);

    if (state & RX_DR)
    {
        // 有新数据则把 32 字节读出来
        SI24R1_Read_Buf(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH);

        // 读完后清空 RX FIFO，避免旧数据残留
        SI24R1_Write_Reg(FLUSH_RX, 0xff);
        return 0;
    }

    // 没有收到新数据
    return 1;
}

uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status, byte_ctr;

    SPI_NSS_LOW;

    status = SPI_RW(reg);
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
    {
        pBuf[byte_ctr] = SPI_RW(0);
    }
    SPI_NSS_HIGH;

    return status;
}
