#ifndef __DRI_NVS_H__
#define __DRI_NVS_H__

#include "nvs_flash.h"
#include "Com_Debug.h"


// 1.NVS分区初始化
void Dri_NVS_Init(void);

// 2.nvs分区nvs_set_str用于存储管理密码
esp_err_t Dri_NVS_SetStr(char *key, char *val);
// 3.nvs分区nvs_get_str用于获取管理密码
esp_err_t Dri_NVS_GetStr(char *key, char *outer_buffer, size_t *len);


// NVS分区nvs_set_u8存储用户密码
esp_err_t Dri_NVS_SetU8(char *key, uint8_t val);
// NVS分区nvs_get_u8获取用户密码
esp_err_t Dri_NVS_GetU8(char *key, uint8_t *val);


//删除用户密码
esp_err_t Dri_NVS_DeleteU8(char *key);
//删除全部用户
esp_err_t Dri_NVS_DeleteAll(void);


//查找普通用户密码是否存在
esp_err_t Dri_NVS_FindPWD(char *key);

#endif /* __DRI_NVS_H__ */
