#include "done_task.h"

// 电源保持任务：通过周期性拉动电源管理引脚，避免外部供电管理电路进入休眠
#define POWER_TASK_NAME "power_task"
#define POWER_TASK_StackDepth 128
#define POWER_TASK_uxPriority 4
TaskHandle_t power_task_handle;
void power_task(void *pvParameters);

// LED 状态显示任务：根据遥控连接状态和飞机状态控制 4 颗 LED
#define LED_TASK_NAME "led_task"
#define LED_TASK_StackDepth 128
#define LED_TASK_uxPriority 3
TaskHandle_t led_task_handle;
void led_task(void *pvParameters);

// 电机任务：当前版本只负责上电时把 4 路电机输出初始化为 0
#define MOTOR_TASK_NAME "motor_task"
#define MOTOR_TASK_StackDepth 128
#define MOTOR_TASK_uxPriority 4
TaskHandle_t motor_task_handle;
void motor_task(void *pvParameters);

// 接收任务：持续从 SI24R1 无线模块读取遥控器发来的控制包
#define RX_TASK_NAME "rx_task"
#define RX_TASK_StackDepth 128
#define RX_TASK_uxPriority 3
TaskHandle_t rx_task_handle;
void rx_task(void *pvParameters);

/* 全局状态区 START */
// 遥控连接状态：当前默认认为是已连接，后续可以加入“超时判定”动态修改
RC_STATUS_E rc_status = eRC_UNCONNECTED;

// 飞机工作状态：当前默认认为是正常状态，后续建议由统一状态机维护
PLANE_STATUS_E plane_status = eDONE_NORMAL;
/* 全局状态区 END */

void done_task_Init(void)
{
    // 系统启动后先等一小段时间，让外设和无线模块稳定下来
    HAL_Delay(200);
    printf("nice done\r\n");

    // 初始化 4 路电机 PWM 输出通道
    Int_Motor_Init();

    // 初始化 SI24R1 无线模块底层引脚和 SPI 接口
    Int_SI4R1_Init();

    // 切换到接收模式，准备接收遥控器发来的控制数据
    SI24R1_RX_Mode();

    // 创建电源保持任务
    xTaskCreate(power_task, POWER_TASK_NAME, POWER_TASK_StackDepth, NULL, POWER_TASK_uxPriority, &power_task_handle);

    // 创建状态灯任务
    xTaskCreate(led_task, LED_TASK_NAME, LED_TASK_StackDepth, NULL, LED_TASK_uxPriority, &led_task_handle);

    // 创建电机任务
    xTaskCreate(motor_task, MOTOR_TASK_NAME, MOTOR_TASK_StackDepth, NULL, MOTOR_TASK_uxPriority, &motor_task_handle);

    // 创建遥控接收任务
    xTaskCreate(rx_task, RX_TASK_NAME, RX_TASK_StackDepth, NULL, RX_TASK_uxPriority, &rx_task_handle);

    // 启动 FreeRTOS 调度器，之后所有业务逻辑都在任务里运行
    vTaskStartScheduler();
}

void power_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        // 这里通过周期性输出低脉冲给 NRST_BAT 引脚，起到“电源保持/防休眠”的作用
        vTaskDelay(1000);
        HAL_GPIO_WritePin(NRST_BAT_GPIO_Port, NRST_BAT_Pin, GPIO_PIN_RESET);

        // 保持 100ms 的低电平脉冲，确保外部电路能识别到
        vTaskDelay(100);
        HAL_GPIO_WritePin(NRST_BAT_GPIO_Port, NRST_BAT_Pin, GPIO_PIN_SET);
    }
}

void led_task(void *pvParameters)
{
    (void)pvParameters;

    // 记录上一次底部 LED 翻转时间，用来做不同频率的闪烁
    uint32_t current_last_time = xTaskGetTickCount();

    while (1)
    {
        // 顶部两颗灯表示遥控器连接状态
        if (rc_status == eRC_UNCONNECTED)
        {
            // 未连接：顶部灯灭
            Int_LED_off(&led_left_top_handler);
            Int_LED_off(&led_right_top_handler);
        }
        else if (rc_status == eRC_CONNECTED)
        {
            // 已连接：顶部灯亮
            Int_LED_on(&led_left_top_handler);
            Int_LED_on(&led_right_top_handler);
        }

        // 底部两颗灯主要表示飞机自身工作状态
        if (plane_status == eDONE_IDEL)
        {
            // 空闲状态：1 秒闪一次
            if (xTaskGetTickCount() - current_last_time >= 1000)
            {
                Int_LED_toggle(&led_left_bottom_handler);
                Int_LED_toggle(&led_right_bottom_handler);
                current_last_time = xTaskGetTickCount();
            }
        }
        else if (plane_status == eDONE_NORMAL)
        {
            // 正常状态：100ms 闪一次，看起来会更快
            if (xTaskGetTickCount() - current_last_time >= 100)
            {
                Int_LED_toggle(&led_left_bottom_handler);
                Int_LED_toggle(&led_right_bottom_handler);
                current_last_time = xTaskGetTickCount();
            }
        }
        else if (plane_status == eDONE_HOLD_HEIGHT)
        {
            // 定高状态：当前版本直接让顶部灯保持亮
            Int_LED_on(&led_left_top_handler);
            Int_LED_on(&led_right_top_handler);
        }
        else if (plane_status == eDONE_FAULT)
        {
            // 故障状态：当前版本直接让顶部灯灭
            Int_LED_off(&led_left_top_handler);
            Int_LED_off(&led_right_top_handler);
        }

        // LED 显示不需要特别高频，50ms 刷新一次足够
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void motor_task(void *pvParameters)
{
    (void)pvParameters;

    // 上电后先把 4 路电机目标速度全部置 0，避免未知占空比直接输出
    motor_left0_top_handle.speed = 0;
    motor_right0_top_handle.speed = 0;
    motor_left1_bottom_handle.speed = 0;
    motor_right1_bottom_handle.speed = 0;

    while (1)
    {
        // 把初始值真正写进 PWM 比较寄存器
        Int_Motor_SetSpeed(&motor_left0_top_handle);
        Int_Motor_SetSpeed(&motor_right0_top_handle);
        Int_Motor_SetSpeed(&motor_left1_bottom_handle);
        Int_Motor_SetSpeed(&motor_right1_bottom_handle);
        // 当前版本这里还没有真正接入“遥控输入 -> 混控 -> 电机输出”的主逻辑。
        // 所以这个任务现在只是执行层的占位框架。
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void rx_task(void *pvParameters)
{
    (void)pvParameters;

    // 用固定节拍轮询接收，避免普通 delay 因执行时间累积误差
    uint32_t last_wake_time = xTaskGetTickCount();
    uint32_t invalid_count = 0;
    while (1)
    {
        // 从无线模块读取一包数据，并尝试解析成 joystick_key_handle 结构体
        VALID_E valid = App_Data_Receive(&joystick_key_handle);

        // 如果数据有效，更新遥控器状态
        if (valid == eVALID)
        {
            invalid_count = 0;
            rc_status = eRC_CONNECTED;
        }
        else if (valid == eINVALID)
        {
            invalid_count++;
            if (invalid_count >= 50)
            {
                rc_status = eRC_UNCONNECTED;
            }
        }

        // 调试打印：确认接收任务正在运行
        // printf("radio_task\n");

        // 当前接收节拍是 10ms，一直轮询无线接收缓冲区
        xTaskDelayUntil(&last_wake_time, 10 / portTICK_PERIOD_MS);
    }
}
