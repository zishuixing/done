#ifndef __INT_LED_H__
#define __INT_LED_H__
#include "main.h"

// LED 句柄：统一描述一颗 LED 的 GPIO 端口和引脚
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

} Led_handler_t;

// 4 颗 LED 的全局句柄
extern Led_handler_t led_left_top_handler;
extern Led_handler_t led_right_top_handler;
extern Led_handler_t led_right_bottom_handler;
extern Led_handler_t led_left_bottom_handler;

// 熄灭指定 LED
void Int_LED_off(Led_handler_t *led_handler);

// 点亮指定 LED
void Int_LED_on(Led_handler_t *led_handler);

// 翻转指定 LED 当前状态
void Int_LED_toggle(Led_handler_t *led_handler);

#endif /* __INT_LED_H__ */
