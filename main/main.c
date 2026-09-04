#include <stdio.h>
#include "Com_Debug.h"

// 语音驱动
#include "Int_WTN6170.h"
// 按键驱动
#include "Int_SC12B.h"
// 灯带驱动
#include "Int_WS2812.h"



// 时基
#define MY_TIME_BASE (50 / portTICK_PERIOD_MS)

// 计数器
int my_time = 0;
// 超时时间，延时3秒
int led_timeOut = 3000 / MY_TIME_BASE;
// 灯带状态
bool led_state = false;


void app_main(void)
{
   // 1.初始化WTN6170模块，语音播报
   Int_WTN6170_Init();
   // 设置音量
   setVolume(3);

   // 2.初始化SC12B模块，按键驱动
   Int_SC12B_Init();

   // 3.初始化WS2812模块，灯带驱动
   Int_WS2812_Init();


   // 4.测试按键
   Key_Number_T key_num = KEY_NULL;
   while(1)
   {
      key_num = Int_SC12B_GetKey();
      if (key_num != KEY_NULL)
      {
         sayWithoutInt(); // 连码播放
         sayWaterDrop(); // 播放水滴声
         MY_LOGI("按键编号: %d", key_num);

         Int_WS2812_LEDOff(); // 清空灯带
         Int_WS2812_LEDOn(key_num, violet); // 点亮对应编号的灯
         my_time = 0; // 重置超时计数
         led_state = true; // 亮灯
      }
      else{
         if (led_state)
         {
            my_time++;
         }
         if (my_time >= led_timeOut)
         {
            Int_WS2812_LEDOff(); // 清空灯带
            my_time = 0; // 重置超时计数
            led_state = false; // 熄灭灯
         }
      }
      vTaskDelay(MY_TIME_BASE);
   }
}
