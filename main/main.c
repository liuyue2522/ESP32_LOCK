#include <stdio.h>
#include "Com_Debug.h"
#include "App_Main.h"


// 1、任务1的控制块
TaskHandle_t task1;
// 2、任务2的控制块
TaskHandle_t task2;

/*******************************************************************************************/

// 1、任务1的回调：专门处理密码相关
void task_CallBack(void *pvParameters);

// 2.任务2的回调函数：专门处理指纹相关
void task_Fingerprint(void *pvParameters);

/*******************************************************************************************/

void app_main(void)
{
   // 1.应用初始化
    App_Main_Init();

    /* // 2.创建一个任务,用于获取用户输入内容(持续判断)
    xTaskCreate(task_CallBack, "task_CallBack", 2048, NULL, 10, &task1);

    // 3.创建一个任务,处理指纹相关业务
    xTaskCreate(task_Fingerprint, "task_Fingerprint", 2048, NULL, 10, &task2); */


    vTaskDelay(3000 / portTICK_PERIOD_MS);

    uint8_t id = Int_FPM383_GetMinID();
    printf("获取指纹库中还未注册ID的最小值 = %d\n", id);

    if (Int_FPM383_AddUserFingerprint(id) == STATE_OK)
    {
        printf("注册指纹成功\n");
    }
    else
    {
        printf("注册指纹失败\n");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    if (Int_FPM383_VerifyFingerprint(&id) == STATE_OK)
    {
        printf("验证指纹成功: %d\n", id);
    }
    else
    {
        printf("验证指纹失败\n");
    }
}


/*******************************************************************************************/

void task_CallBack(void *pvParameters)
{
    char buffers[BUFFER_SIZE];

    // 每隔50ms执行一次,检测用户是否按下按键,如果按下按键。收集用户输入内容!!!!!!!
    while (1)
    {
        // 获取用户输入内容
        STATE_T result = App_Main_GetUserInput(buffers);
        switch (result)
        {
        case STATE_ERROR: // 按下M键,非法操作
            MY_LOGE("按下M");
            break;
        case STATE_OK: // 按下#
            MY_LOGE("按下#,收集到内容:%s", buffers);
            App_Main_Handler(buffers);
            break;
        case STATE_TIMEOUT: // 超时
            // MY_LOGE("超时");
            // Int_WS2812_LEDOff();
            break;
        default:
            break;
        }

        // 清空每一次用户输入内容
        memset(buffers, 0, sizeof(buffers));
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void task_Fingerprint(void *pvParameters)
{
    while (1)
    {
        // 任务1 内部处理指纹业务
        App_Main_Handler_FingerPrint();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
