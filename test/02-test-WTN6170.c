#include <stdio.h>
#include "Int_BDR6120.h"
#include "Int_WTN6170.h"


void app_main(void)
{
   //1.初始化WTN6170模块
   Int_WTN6170_Init();

   //3.设置音量
   setVolume(1); // 设置音量为1

   //2.测试语音
   sayWithoutInt(); // 连码播放
   sayFail(); // 失败音效

   sayWithoutInt(); // 连码播放
   sayVerifyFail(); // 验证失败

   sayWithoutInt(); // 连码播放
   sayRetry(); // 重试
}
