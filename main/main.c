#include <stdio.h>
#include "Int_BDR6120.h"
#include "Int_WTN6170.h"


void app_main(void)
{
   //1.初始化WTN6170模块
   Int_WTN6170_Init();


   //2.测试语音
   sayWithoutInt(); // 连码播放
   sayDelUserFingerprint(); // 删除用户指纹
   sayWithoutInt(); // 连码播放
   sayDelSucc(); // 删除成功
}


/* void app_main(void)
{
   //1.初始化一次
   Int_BDR6120_Init();

   //2.开门
   Int_BDR6120_Open();
} */
