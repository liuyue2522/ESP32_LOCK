#ifndef __APP_BLE_H__
#define __APP_BLE_H__
#include "Dri_BLE.h"
#include "stdint.h"
#include "esp_log.h"
#include  "Dri_NVS.h"
#include "Int_WTN6170.h"
#include "Int_BDR6120.h"
#include "string.h"

//1.蓝牙驱动初始化
void App_BLE_Init(void);
#endif /* __APP_BLE_H__ */