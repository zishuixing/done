#include "Int_Led.h"

Led_handler_t led_left_top_handler = {.GPIO_Pin = LED1_Pin, .GPIOx = LED1_GPIO_Port};
Led_handler_t led_right_top_handler = {.GPIO_Pin = LED2_Pin, .GPIOx = LED2_GPIO_Port};

Led_handler_t led_right_bottom_handler = {.GPIO_Pin = LED3_Pin, .GPIOx = LED3_GPIO_Port};
Led_handler_t led_left_bottom_handler = {.GPIO_Pin = LED4_Pin, .GPIOx = LED4_GPIO_Port};

void Int_LED_off(Led_handler_t *led_handler)
{
    HAL_GPIO_WritePin(led_handler->GPIOx, led_handler->GPIO_Pin, GPIO_PIN_SET);
}

void Int_LED_on(Led_handler_t *led_handler)
{
    HAL_GPIO_WritePin(led_handler->GPIOx, led_handler->GPIO_Pin, GPIO_PIN_SET);
}

// 翻转led灯光
void Int_LED_toggle(Led_handler_t *led_handler)
{
    HAL_GPIO_TogglePin(led_handler->GPIOx, led_handler->GPIO_Pin);
}
