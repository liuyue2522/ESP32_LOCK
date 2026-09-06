#include "dri_NVS.h"


// NVS句柄初始化
nvs_handle_t my_handler;


void Dri_NVS_Init(void)
{
    // 1.FLASH的NVS分区初始化
    esp_err_t err = nvs_flash_init();

    // 2.如果初始化失败，则重新初始化
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) // NVS分区错误：没有空闲页面或版本不匹配
    {
        MY_LOGE("NVS分区错误：没有空闲页面或版本不匹配");
        MY_LOGE("NVS分区错误：开始擦除NVS分区");

        ESP_ERROR_CHECK(nvs_flash_erase()); // 擦除NVS分区
        err = nvs_flash_init(); // 重新初始化NVS分区
    }
    ESP_ERROR_CHECK( err ); // 检查错误

    // 3.NVS分区初始化一块空间,持久化存储用户数据
    err = nvs_open("USER_DATA", NVS_READWRITE, &my_handler);
    if (err == ESP_OK)
    {
        MY_LOGI("NVS分区初始化成功");
    }
    else
    {
        MY_LOGI("NVS分区初始化失败");
    }
}

// 写入字符串
esp_err_t Dri_NVS_SetValueByKey(char *key, char *val)
{
    return nvs_set_str(my_handler, key, val);
}

// 读取字符串
esp_err_t Dri_NVS_GetValueByKey(char *key, char *val, size_t *len)
{
    return nvs_get_str(my_handler, key, val, len);
}

// 写入数字
esp_err_t Dri_NVS_SetKey(char *key, uint8_t val)
{
    return nvs_set_u8(my_handler, key, val);
}

// 读取数字
esp_err_t Dri_NVS_GetKey(char *key, uint8_t *val)
{
    return nvs_get_u8(my_handler, key, val);
}

// 删除信息
esp_err_t Dri_NVS_DeleteKey(char *key)
{
    return nvs_erase_key(my_handler, key);
}
// 删除全部用户
esp_err_t Dri_NVS_DeleteAll(void)
{
    return nvs_flash_erase(); // 擦除NVS分区，删除 NVS 所有数据
}

// 判断普通用户密码是否存在
esp_err_t Dri_NVS_FindKey(char *key)
{
    return nvs_find_key(my_handler, key, NULL);
}
