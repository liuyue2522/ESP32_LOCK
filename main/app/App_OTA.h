#ifndef __APP_OTA_H__
#define __APP_OTA_H__
#include "Dri_wifi_sta.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "string.h"
#include "esp_crt_bundle.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>
#include "esp_wifi.h"
#define HASH_LEN 32
//1.OTA在线升级
void App_OTA_Init(void);


#endif /* __APP_OTA_H__ */