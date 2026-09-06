#ifndef __DRI_NVS_H__
#define __DRI_NVS_H__

#include "nvs_flash.h"
#include "Com_Debug.h"


// 1.NVS分区初始化
void Dri_NVS_Init(void);

// 2.nvs分区nvs_set_str用于存储管理密码
esp_err_t Dri_NVS_SetValueByKey(char *key, char *val);
// 3.nvs分区nvs_get_str用于获取管理密码
esp_err_t Dri_NVS_GetValueByKey(char *key, char *outer_buffer, size_t *len);


// NVS分区nvs_set_u8存储用户密码
esp_err_t Dri_NVS_SetKey(char *key, uint8_t val);
// NVS分区nvs_get_u8获取用户密码
esp_err_t Dri_NVS_GetKey(char *key, uint8_t *val);


//删除用户密码
esp_err_t Dri_NVS_DeleteKey(char *key);
//删除全部用户
esp_err_t Dri_NVS_DeleteAll(void);


//查找普通用户密码是否存在
esp_err_t Dri_NVS_FindKey(char *key);

#endif /* __DRI_NVS_H__ */
