#include "Int_Led.h"

// 4 颗 LED 的硬件句柄。
// 这样任务层只需要关心“左上灯/右上灯/左下灯/右下灯”，不用反复记引脚。
Led_handler_t led_left_top_handler = {.GPIO_Pin = LED1_Pin, .GPIOx = LED1_GPIO_Port};
Led_handler_t led_right_top_handler = {.GPIO_Pin = LED2_Pin, .GPIOx = LED2_GPIO_Port};
Led_handler_t led_right_bottom_handler = {.GPIO_Pin = LED3_Pin, .GPIOx = LED3_GPIO_Port};
Led_handler_t led_left_bottom_handler = {.GPIO_Pin = LED4_Pin, .GPIOx = LED4_GPIO_Port};

void Int_LED_off(Led_handler_t *led_handler)
{
    // 当前硬件接法下，这里的高电平被当成“熄灭”来使用。
    HAL_GPIO_WritePin(led_handler->GPIOx, led_handler->GPIO_Pin, GPIO_PIN_SET);
}

void Int_LED_on(Led_handler_t *led_handler)
{
    // 注意：从你当前代码现状来看，这里和 off 一样也写成了 SET。
    // 我这次不改逻辑，只保留现状并通过注释提醒后续维护时重点确认“有效电平”。
    HAL_GPIO_WritePin(led_handler->GPIOx, led_handler->GPIO_Pin, GPIO_PIN_SET);
}

void Int_LED_toggle(Led_handler_t *led_handler)
{
    // 翻转当前 LED 电平，用于做闪烁效果
    HAL_GPIO_TogglePin(led_handler->GPIOx, led_handler->GPIO_Pin);
}
