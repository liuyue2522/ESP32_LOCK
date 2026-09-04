#ifndef __COM_DEBUG_H__
#define __COM_DEBUG_H__

#include "esp_log.h"
#include "string.h"

#define DEBUG 1

extern char tag[100];

// 开发阶段
#if(DEBUG == 1)
// E（错误）、W（警告）、I（信息）、D（调试）、V（详细）。   默认为I。

#define MY_LOGE(format, ...)                                \
    do {                                                    \
        sprintf(tag, "[%s:%d]", __FILENAME__, __LINE__); \
        ESP_LOGE(tag, format, ##__VA_ARGS__);               \
    } while(0)
#define MY_LOGW(format, ...)                                \
    do {                                                    \
        sprintf(tag, "[%s:%d]", __FILENAME__, __LINE__); \
        ESP_LOGW(tag, format, ##__VA_ARGS__);               \
    } while(0)

#define MY_LOGI(format, ...)                                \
    do {                                                    \
        sprintf(tag, "[%s:%d]", __FILENAME__, __LINE__); \
        ESP_LOGI(tag, format, ##__VA_ARGS__);               \
    } while(0)

#define MY_LOGD(format, ...)                                \
    do {                                                    \
        sprintf(tag, "[%s:%d]", __FILENAME__, __LINE__); \
        ESP_LOGD(tag, format, ##__VA_ARGS__);               \
    } while(0)

#define MY_LOGV(format, ...)                                \
    do {                                                    \
        sprintf(tag, "[%s:%d]", __FILENAME__, __LINE__); \
        ESP_LOGV(tag, format, ##__VA_ARGS__);               \
    } while(0)
#else
#define MY_LOGE(format, ...)
#define MY_LOGW(format, ...)
#define MY_LOGI(format, ...)
#define MY_LOGD(format, ...)
#define MY_LOGV(format, ...)
#endif

#endif /* __COM_DEBUG_H__ */
