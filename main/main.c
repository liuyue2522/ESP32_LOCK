#include <stdio.h>
#include "Com_Debug.h"

// 语音驱动
#include "Int_WTN6170.h"
// 按键驱动
#include "Int_SC12B.h"


void app_main(void)
{
   // 1.初始化WTN6170模块，语音播报
   Int_WTN6170_Init();
   // 设置音量
   setVolume(3);

   // 2.初始化SC12B模块，按键驱动
   Int_SC12B_Init();


   // 3.测试按键
   Key_Number_T key_num = KEY_NULL;
   while(1)
   {
      key_num = Int_SC12B_GetKey();
      if (key_num != KEY_NULL)
      {
         sayWithoutInt(); // 连码播放
         sayWaterDrop(); // 播放水滴声
         MY_LOGI("按键编号: %d", key_num);
      }
      vTaskDelay(50 / portTICK_PERIOD_MS);
   }
}
