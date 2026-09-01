#ifndef __INT_BDR6120_H__
#define __INT_BDR6120_H__

#include "driver/gpio.h"
#include "esp_task.h"

#define BDR_A GPIO_NUM_4
#define BDR_B GPIO_NUM_5

// 1.初始化方法
void Int_BDR6120_Init(void);


//2.开锁方法
void Int_BDR6120_Open(void);

#endif /* __INT_BDR6120_H__ */
