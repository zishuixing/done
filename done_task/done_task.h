#ifndef __DONE_TASK_H__
#define __DONE_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "Common_types.h"
#include "Int_Led.h"
#include "Int_Motor.h"
#include "Int_SI4R1.h"
#include "App_Data.h"

// 系统任务初始化入口。
// main() 在完成外设初始化后调用这里，随后由这里统一创建各个 FreeRTOS 任务。
void done_task_Init(void);

#endif /* __DONE_TASK_H__ */
