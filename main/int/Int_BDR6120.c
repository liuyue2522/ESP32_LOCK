#include "Int_BDR6120.h"

// 待机状态
static void Int_BDR6120_Standby(void);
// 前进
static void Int_BDR6120_Forward(void);
// 后退
static void Int_BDR6120_Backward(void);
// 刹车
static void Int_BDR6120_Brake(void);



void Int_BDR6120_Init(void)
{
    // 片上外设GPIO初始化
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,              // 不需要外部中断
        .mode = GPIO_MODE_OUTPUT,                    // 输出模式
        .pin_bit_mask = (1 << BDR_A) | (1 << BDR_B), // 设置引脚掩码
        .pull_down_en = GPIO_PULLDOWN_DISABLE,       // 不需要下拉
        .pull_up_en = GPIO_PULLUP_DISABLE,           // 不需要上拉
    };

    // 初始化IO引脚
    gpio_config(&io_conf);

    // 待机处于待命状态:BDR6120驱动芯片与电机断开状态
    Int_BDR6120_Standby();
}

// 开锁方法
void Int_BDR6120_Open(void)
{
    //1.电机前进
    Int_BDR6120_Forward();
    vTaskDelay(1000);

    //2.让用户进入,等以后再把门锁上
    Int_BDR6120_Brake();
    vTaskDelay(3000);

    //3.电机后退
    Int_BDR6120_Backward();
    vTaskDelay(1000);

    //4.电机刹车
    Int_BDR6120_Brake();
}

// 待机状态
static void Int_BDR6120_Standby(void)
{
    gpio_set_level(BDR_A, 0);
    gpio_set_level(BDR_B, 0);
}
// 前进
static void Int_BDR6120_Forward(void)
{
    gpio_set_level(BDR_A, 1);
    gpio_set_level(BDR_B, 0);
}
// 后退
static void Int_BDR6120_Backward(void)
{
    gpio_set_level(BDR_A, 0);
    gpio_set_level(BDR_B, 1);
}
// 刹车
static void Int_BDR6120_Brake(void)
{
    gpio_set_level(BDR_A, 1);
    gpio_set_level(BDR_B, 1);
}
