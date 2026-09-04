#include <stdio.h>
#include "Com_Debug.h"
#include "App_Main.h"


// 1、任务的回调
void task_CallBack(void *pvParameters);

void app_main(void)
{
   // 1.应用初始化
    App_Main_Init();

    // 2.创建一个任务,用于获取用户输入内容(持续判断)
    xTaskCreate(task_CallBack, "task_CallBack", 2048, NULL, 10, NULL);
}

void task_CallBack(void *pvParameters)
{
    char buffers[20];

    // 每隔50ms执行一次,检测用户是否按下按键,如果按下按键。收集用户输入内容!!!!!!!
    while (1)
    {
        // 获取用户输入内容
        Status_T result = App_Main_GetUserInput(buffers);
        switch (result)
        {
            // 按下M键,非法操作
        case Status_ERROR:
            MY_LOGE("按下M");
            break;
            // 按下#
        case Status_OK:
            MY_LOGE("按下#,收集到内容:%s", buffers);
            if (strcmp(buffers, "123456") == 0)
            {
               MY_LOGE("密码正确");
               Int_BDR6120_Open();
            }
            break;

        case Status_TIMEOUT:
            MY_LOGE("超时");
            Int_WS2812_LEDOff();
            break;

        default:
            break;
        }

        // 清空每一次用户输入内容
        memset(buffers, 0, sizeof(buffers));
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
