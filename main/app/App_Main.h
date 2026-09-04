#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

/* esp_task.h 是什么？ */
/* 1.集中定义系统任务优先级：为 ESP-IDF 中的各种系统任务（如 Wi-Fi、蓝牙、TCP/IP 等）统一分配优先级。 */
/* 2.集中定义系统任务栈大小：为这些系统任务预设推荐的栈大小。简单来说，它是 ESP-IDF 的“系统任务配置清单”，
    用于确保各个系统任务能够有序、稳定地运行。
 */
#include "esp_task.h"

// 电机接口
#include "Int_BDR6120.h"
// 语音接口
#include "Int_WTN6170.h"
// 按键接口
#include "Int_SC12B.h"
// 灯带接口
#include "Int_WS2812.h"
// NVS驱动
#include "Dri_NVS.h"

#include "Com_Config.h"
#include "string.h"
#include "Com_Debug.h"


// 1.项目初始化方法
void App_Main_Init(void);

// 2.处理获取用户输入内容
Status_T App_Main_GetUserInput(char buffer[]);

#endif /* __APP_MAIN_H__ */
