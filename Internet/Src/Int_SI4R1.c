#include "Int_SI4R1.h"
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01}; // 定义一个静态发送地址

// 硬件 SPI 版本 - 直接使用 HAL 库
static uint8_t SPI_RW(uint8_t byte)
{
    uint8_t rx_data;

    // 使用硬件 SPI 同时发送和接收一个字节
    HAL_SPI_TransmitReceive(&hspi1, &byte, &rx_data, 1, HAL_MAX_DELAY);

    return rx_data;
}
void Int_SI4R1_Init(void)
{

    // 设置初始电平
    SPI_NSS_HIGH; // CSN=1
    SI4R1_CE_LOW; // CE=0
    // IRQ 作为输入，不需要设置电平
}

/********************************************************
函数功能：SI24R1发送模式初始化
入口参数：无
返回  值：无
*********************************************************/
void Int_SI24R1_TX_Mode(void)
{
    // ① 拉低 CE，进入配置模式
    SI4R1_CE_LOW;

    // ② 写入发送地址
    SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);

    // ③ 设置接收通道0地址（用于接收ACK）
    SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);

    // ④ 使能自动应答（仅通道0）
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);

    // ⑤ 使能接收地址通道0
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);

    // ⑥ 自动重发设置：延时250us+86us，重发10次
    SI24R1_Write_Reg(SI24R1_WRITE_REG + SETUP_RETR, 0x0a);

    // ⑦ 选择射频通道 2440MHz
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, 40);

    // ⑧ 配置射频：2Mbps，7dBm发射功率
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x0f);

    // ⑨ 配置芯片：CRC使能，16位CRC，上电，发送模式
    SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0e);

    // 注意：CE 没有拉高，保持低电平，等待发送数据时再拉高
}

// 写单个寄存器
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;

    SPI_NSS_LOW;          // CSN=0
    status = SPI_RW(reg); // 发送命令，同时接收状态
    SPI_RW(value);        // 发送数据
    SPI_NSS_HIGH;         // CSN=1

    return status;
}

/********************************************************
函数功能：写寄存器的值（多字节）
入口参数：reg:寄存器映射地址（格式：SI24R1_WRITE_REG｜reg）
                    pBuf:写数据首地址
                    bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status, byte_ctr;

    SPI_NSS_LOW; // CSN=0

    status = SPI_RW(reg);
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
        SPI_RW(*pBuf++);

    SPI_NSS_HIGH; // CSN=1

    return (status);
}

/********************************************************
函数功能：发送一个数据包
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败
          0x20:发送成功
          0xff:发送失败
*********************************************************/
uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
    uint8_t state;
    // ① 拉低 CE，进入配置模式
    SI4R1_CE_LOW;
    // CE拉低，使能SI24R1配置
    SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // 写数据到TX FIFO,32个字节
    SI4R1_CE_HIGH;
    // CE置高，使能发送

    while (HAL_GPIO_ReadPin(SI_IRO_GPIO_Port, SI_IRO_Pin) == 1)
        ;                                               // 等待发送完成
    state = SI24R1_Read_Reg(STATUS);                    // 读取状态寄存器的值
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state); // 清除TX_DS或MAX_RT中断标志
    if (state & MAX_RT)                                 // 达到最大重发次数
    {
        SI24R1_Write_Reg(FLUSH_TX, 0xff); // 清除TX FIFO寄存器
        return MAX_RT;
    }
    if (state & TX_DS) // 发送完成
    {
        return TX_DS;
    }
    return 0XFF; // 发送失败
}

/********************************************************
函数功能：读取寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
uint8_t SI24R1_Read_Reg(uint8_t reg)
{
    uint8_t value;

    SPI_NSS_LOW; // CSN=0

    SPI_RW(reg);
    value = SPI_RW(0);
    SPI_NSS_HIGH; // CSN=1

    return (value);
}

/********************************************************
函数功能：SI24R1接收模式初始化
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_RX_Mode(void)
{
    SI4R1_CE_LOW;                                                              // CSN=0
    SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 接收设备接收通道0使用和发送设备相同的发送地址
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);                          // 使能接收通道0自动应答
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);                      // 使能接收通道0
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, 40);                            // 选择射频通道0x40
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);             // 接收通道0选择和发送通道相同有效数据宽度
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x0f);                       // 数据传输率2Mbps，发射功率7dBm
    SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0f);                         // CRC使能，16位CRC校验，上电，接收模式
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, 0xff);                         // 清除所有的中断标志位
    SI4R1_CE_HIGH;                                                             // CSN=1                                      									// 拉高CE启动接收设备
}

/********************************************************
函数功能：读取接收数据
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
    uint8_t state;
    state = SI24R1_Read_Reg(STATUS);                    // 读取状态寄存器的值
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state); // 清除RX_DS中断标志

    if (state & RX_DR) // 接收到数据
    {
        SI24R1_Read_Buf(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH); // 读取数据
        SI24R1_Write_Reg(FLUSH_RX, 0xff);                    // 清除RX FIFO寄存器
        return 0;
    }
    return 1; // 没收到任何数据
}

/********************************************************
函数功能：读取寄存器的值（多字节）
入口参数：reg:寄存器映射地址（READ_REG｜reg）
                    pBuf:接收缓冲区的首地址
                    bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status, byte_ctr;

    SPI_NSS_LOW; // CSN=0

    status = SPI_RW(reg);
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0); // 读取数据，低字节在前
    SPI_NSS_HIGH;                   // CSN=1                                      									// 拉高CE启动接收设备

    return (status);
}
