#ifndef __DRI_BLE_H__
#define __DRI_BLE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HRPS_HT_MEAS_MAX_LEN (13)
#define HRPS_MANDATORY_MASK (0x0F)
#define HRPS_BODY_SENSOR_LOC_MASK (0x30)
#define HRPS_HR_CTNL_PT_MASK (0xC0)
enum
{
    HRS_IDX_SVC,
    HRS_IDX_HR_MEAS_CHAR,
    HRS_IDX_HR_MEAS_VAL,
    HRS_IDX_HR_MEAS_NTF_CFG,
    HRS_IDX_BOBY_SENSOR_LOC_CHAR,
    HRS_IDX_BOBY_SENSOR_LOC_VAL,
    HRS_IDX_HR_CTNL_PT_CHAR,
    HRS_IDX_HR_CTNL_PT_VAL,
    HRS_IDX_NB,
};



//蓝牙驱动初始化方法
void Dri_BLE_Init(void);
#endif /* __DRI_BLE_H__ */