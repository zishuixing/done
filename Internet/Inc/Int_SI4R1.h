#ifndef __INT_SI4R1_H__
#define __INT_SI4R1_H__
#include "spi.h"
#include "main.h"

// 无线地址宽度：当前使用 5 字节地址
#define TX_ADR_WIDTH 5

// 每包固定 32 字节载荷
#define TX_PLOAD_WIDTH 32

// SPI(SI24R1) 命令字
#define SI24R1_READ_REG 0x00
#define SI24R1_WRITE_REG 0x20
#define RD_RX_PLOAD 0x61
#define WR_TX_PLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define NOP 0xFF

// SI24R1 寄存器地址
#define CONFIG 0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_AW 0x03
#define SETUP_RETR 0x04
#define RF_CH 0x05
#define RF_SETUP 0x06
#define STATUS 0x07
#define OBSERVE_TX 0x08
#define RSSI 0x09
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17

// STATUS 寄存器关键标志位
#define RX_DR 0x40
#define TX_DS 0x20
#define MAX_RT 0x10

// 片选和 CE 控制宏
#define SPI_NSS_LOW HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_RESET);
#define SPI_NSS_HIGH HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_SET);
#define SI4R1_CE_LOW HAL_GPIO_WritePin(SI_EN_GPIO_Port, SI_EN_Pin, GPIO_PIN_RESET);
#define SI4R1_CE_HIGH HAL_GPIO_WritePin(SI_EN_GPIO_Port, SI_EN_Pin, GPIO_PIN_SET);

// 初始化 SI24R1 的基础引脚状态
void Int_SI4R1_Init(void);

// 写单字节寄存器
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value);

// 连续写多个字节到指定寄存器或 FIFO
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes);

// 读单字节寄存器
uint8_t SI24R1_Read_Reg(uint8_t reg);

// 连续读多个字节
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);

// 切换到接收模式
void SI24R1_RX_Mode(void);

// 切换到发送模式
void Int_SI24R1_TX_Mode(void);

// 从无线模块读出一包 32 字节数据
uint8_t SI24R1_RxPacket(uint8_t *rxbuf);

// 向无线模块发送一包 32 字节数据
uint8_t SI24R1_TxPacket(uint8_t *txbuf);

#endif /* __INT_SI4R1_H__ */
