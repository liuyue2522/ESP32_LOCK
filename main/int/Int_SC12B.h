#ifndef __INT_SC12B_H__
#define __INT_SC12B_H__

#include "Com_Debug.h"
#include "driver/i2c.h"

#define SC_INT GPIO_NUM_0
#define SC_SDA GPIO_NUM_2
#define SC_SCL GPIO_NUM_1

//从机地址
#define SLAVE_ADDR 0x42
// I2C通信速度
#define I2C_MASTER_FREQ_HZ 400000
/*!< I2C master doesn't need buffer */
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

typedef enum
{
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_HASH, // #
    KEY_M, // M
    KEY_NULL // 没有按键按下
} Key_Number_T;

// 1.初始化方法
void Int_SC12B_Init(void);

// 2.获取用户按下按键编号
Key_Number_T Int_SC12B_GetKey(void);


#endif /* __INT_SC12B_H__ */
