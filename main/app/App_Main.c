#include "App_Main.h"

// 时基
#define MY_TIME_BASE (20 / portTICK_PERIOD_MS)

// 计数器
uint32_t my_timer = 0;

// 超时时间，延时3秒
int time_out = 3000 / MY_TIME_BASE;


// 1.项目初始化
void App_Main_Init(void)
{
    // 1.电机
    Int_BDR6120_Init();
    // 2.语音
    Int_WTN6170_Init();
    // 设置音量
   setVolume(3);
    // 3.按键
    Int_SC12B_Init();
    // 4.背光灯
    Int_WS2812_Init();
    // 5.NVS分区持久化存储用户数据
    Dri_NVS_Init();
}

Status_T App_Main_GetUserInput(char buffer[])
{
    uint8_t index = 0;
    while (1)
    {
        Key_Number_T key_number = Int_SC12B_GetKey();
        // 说明用户根本没有按下按键
        if (key_number == KEY_NULL)
        {
            // 每隔50ms+1
            my_timer++;
            // 1.超时判断,如果按键之间,相差3s,任务超时,收集用户内容行为结束!返回超时
            if (my_timer >= time_out)
            {
                Int_WS2812_LEDOff(); // 清空灯带
                my_timer = 0; // 重置计数器
                return Status_TIMEOUT;
            }
        }
        else
        {
            // 按下按键
            my_timer = 0;

            // 音效
            sayWithoutInt(); // 连码播放
            sayWaterDrop(); // 播放水滴声

            // 按键编号
            MY_LOGI("按键编号: %d", key_number);
            
            // 背光灯
            Int_WS2812_LEDOff(); // 清空灯带
            Int_WS2812_LEDOn(key_number, violet); // 点亮对应编号的灯

            // 按下#
            if (key_number == KEY_HASH)
            {
                Int_WS2812_LEDOff(); // 清空灯带
                return Status_OK;
            }

            // 按下M
            if (key_number == KEY_M)
            {
                Int_WS2812_LEDOff(); // 清空灯带
                return Status_ERROR;
            }

            // 收集用户按下的内容:密码,字符串,字符串本质字符数组,里面内容字符,不是数字!
            buffer[index++] = key_number + 48;
        }

        vTaskDelay(MY_TIME_BASE);
    }
}

