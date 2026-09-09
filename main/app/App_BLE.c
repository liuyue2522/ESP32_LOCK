#include "App_BLE.h"
static char *TAG = "APP_BLE";
// 蓝牙初始化
void App_BLE_Init(void)
{
    Dri_BLE_Init();
}

// 重写ble提供弱函数
void App_Communication_RecvDataCb(uint8_t *data, uint16_t dataLen)
{
    ESP_LOGE(TAG, "APP_BLE_RecvDataCb:%s,len:%d", data, dataLen);
    // 先获取六位密码
    char pwd_array[10] = {0};
    // 获取+后面六位数字
    memcpy(pwd_array, &data[2], 6);

    switch (data[0])
    {
        // 通过蓝牙添加用户密码,持久化存储在FLASH
    case 'A':
        esp_err_t err = Dri_NVS_SetKey(pwd_array, 0);
        if (err == ESP_OK)
        {
            sayWithoutInt();
            sayAddUser();
            sayWithoutInt();
            sayAddSucc();
        }
        else
        {
            sayWithoutInt();
            sayAddUser();
            sayWithoutInt();
            sayAddFail();
        }
        break;
    case 'B':
        esp_err_t err1 = Dri_NVS_DeleteKey(pwd_array);
        if (err1 == ESP_OK)
        {
            sayWithoutInt();
            sayDelUser();
            sayWithoutInt();
            sayDelSucc();
        }
        else
        {
            sayWithoutInt();
            sayDelUser();
            sayWithoutInt();
            sayDelFail();
        }
        break;
    case 'C':
        esp_err_t err2 = Dri_NVS_FindKey(pwd_array);
        if (err2 == ESP_OK)
        {
            sayWithoutInt();
            sayWaterDrop();
            Int_BDR6120_Open();
        }
        else
        {
            sayWithoutInt();
            sayAlarm();
        }
        break;
    default:
        break;
    }
}
