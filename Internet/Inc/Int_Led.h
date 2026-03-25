#ifndef __INT_LED_H__
#define __INT_LED_H__
#include "main.h"

// 创建LED的句柄式结构体
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

} Led_handler_t;

extern Led_handler_t led_left_top_handler;
extern Led_handler_t led_right_top_handler;
extern Led_handler_t led_right_bottom_handler;
extern Led_handler_t led_left_bottom_handler;

void Int_LED_off(Led_handler_t *led_handler);
void Int_LED_on(Led_handler_t *led_handler);
void Int_LED_toggle(Led_handler_t *led_handler);

#endif /* __INT_LED_H__ */
