#include "done_task.h"

// 函数名称
#define POWER_TASK_NAME "power_task"
#define POWER_TASK_StackDepth 128
#define POWER_TASK_uxPriority 4
TaskHandle_t power_task_handle;
void power_task(void *pvParameters);

// 设置灯控任务
#define LED_TASK_NAME "led_task"
#define LED_TASK_StackDepth 128
#define LED_TASK_uxPriority 3
TaskHandle_t led_task_handle;
void led_task(void *pvParameters);

/*全局变量状态区 START*/
// 遥控状态全局变量
RC_STATUS_E rc_status = eRC_CONNECTED;
PLANE_STATUS_E plane_status = eDONE_NORMAL;
/*全局变量状态区 END*/

void done_task_Init(void)
{
    // 创建电源管理任务
    xTaskCreate(power_task, POWER_TASK_NAME, POWER_TASK_StackDepth, NULL, POWER_TASK_uxPriority, &power_task_handle);
    // 创建灯控任务
    xTaskCreate(led_task, LED_TASK_NAME, LED_TASK_StackDepth, NULL, LED_TASK_uxPriority, &led_task_handle);

    // 启动freertos
    vTaskStartScheduler();
}

void power_task(void *pvParameters)
{

    while (1)
    {
        // 每十秒都开机一次 防止低功耗休眠
        vTaskDelay(10000);
        HAL_GPIO_WritePin(NRST_BAT_GPIO_Port, NRST_BAT_Pin, GPIO_PIN_RESET);
        // 延时100ms
        vTaskDelay(100);
        HAL_GPIO_WritePin(NRST_BAT_GPIO_Port, NRST_BAT_Pin, GPIO_PIN_SET);
    }
}

void led_task(void *pvParameters)
{
    uint32_t current_last_time = xTaskGetTickCount();
    while (1)
    {
        if (rc_status == eRC_UNCONNECTED)
        {
            // LED左上角和右上角灯灭
            Int_LED_off(&led_left_top_handler);
            Int_LED_off(&led_right_top_handler);
        }
        else if (rc_status == eRC_CONNECTED)
        {
            // LED左上角和右上角灯亮
            Int_LED_on(&led_left_top_handler);
            Int_LED_on(&led_right_top_handler);
        }

        // 飞机自身状态灯
        // 空闲状态 左下和右下按照一秒闪烁
        if (plane_status == eDONE_IDEL)
        {
            // 要保持刷新率不能变 100ms刷新一次 所以函数中不能写延迟
            if (xTaskGetTickCount() - current_last_time >= 1000)
            {
                // 翻转led灯光
                Int_LED_toggle(&led_left_bottom_handler);
                Int_LED_toggle(&led_right_bottom_handler);
                // 更新值
                current_last_time = xTaskGetTickCount();
            }
        }

        // 正常状态 左下和右下等间隔100ms闪烁
        else if (plane_status == eDONE_NORMAL)
        {

            // 要保持刷新率不能变 100ms刷新一次 所以函数中不能写延迟
            if (xTaskGetTickCount() - current_last_time >= 100)
            {
                // 翻转led灯光
                Int_LED_toggle(&led_left_bottom_handler);
                Int_LED_toggle(&led_right_bottom_handler);
                // 更新值
                current_last_time = xTaskGetTickCount();
            }
        }

        // 定高状态 左上和右上等常亮
        else if (plane_status == eDONE_HOLD_HEIGHT)
        {
            Int_LED_on(&led_left_top_handler);
            Int_LED_on(&led_right_top_handler);
        }
        // 失败状态 左上和右上等关闭
        else if (plane_status == eDONE_FAULT)
        {
            Int_LED_off(&led_left_top_handler);
            Int_LED_off(&led_right_top_handler);
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
