#include <stdio.h>
#include "Com_Debug.h"

// NVS驱动
#include "Dri_NVS.h"


void app_main(void)
{
   char str[100] = {0};
   size_t len = sizeof(str);


   // 初始化NVS
   Dri_NVS_Init();
   // 写入NVS
   if (Dri_NVS_SetStr("test", "hello world") == ESP_OK)
   {
      MY_LOGI("NVS write success");
   }
   else
   {
      MY_LOGI("NVS write fail");
   }
   // 读取NVS
   if (Dri_NVS_GetStr("test", str, &len) == ESP_OK)
   {
      MY_LOGI("NVS read success");
      MY_LOGI("NVS: %s, len: %zu", str, len);
   }
   else
   {
      MY_LOGI("NVS read fail");
   }
   // 擦除NVS
   if (Dri_NVS_DeleteAll() == ESP_OK)
   {
      MY_LOGI("NVS erase success");
   }
   else
   {
      MY_LOGI("NVS erase fail");
   }
   // 读取NVS
   if (Dri_NVS_GetStr("test", str, &len) == ESP_OK)
   {
      MY_LOGI("NVS read success");
      MY_LOGI("NVS: %s, len: %zu", str, len);
   }
   else
   {
      MY_LOGI("NVS read fail");
   }

   /* 
      擦除后，如果想重新使用 NVS，需要再次调用 Dri_NVS_Init() 或 nvs_flash_init()
      （因为 nvs_flash_erase() 后分区被清空，句柄可能仍有效，但后续操作可能出错）。
   */
   // 初始化NVS
   Dri_NVS_Init();
   // 写入NVS
   if (Dri_NVS_SetStr("test", "hello world") == ESP_OK)
   {
      MY_LOGI("NVS write success");
   }
   else
   {
      MY_LOGI("NVS write fail");
   }
   // 读取NVS
   if (Dri_NVS_GetStr("test", str, &len) == ESP_OK)
   {
      MY_LOGI("NVS read success");
      MY_LOGI("NVS: %s, len: %zu", str, len);
   }
   else
   {
      MY_LOGI("NVS read fail");
   }
}
