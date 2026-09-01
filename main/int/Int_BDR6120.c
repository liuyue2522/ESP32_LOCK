#include "Int_BDR6120.h"

// 待机状态
static void Int_BDR6120_Standby(void);
// 前进
static void Int_BDR6120_Forward(void);
// 后退
static void Int_BDR6120_Backward(void);
// 刹车
static void Int_BDR6120_Brake(void);



// 初始化方法
void Int_BDR6120_Init(void)
{
    gpio_config_t io_conf = {0};
    io_conf.pin_bit_mask = (1ULL << BDR_A) | (1ULL << BDR_B);
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);
}

// 开锁方法
void Int_BDR6120_Open(void)
{
    gpio_set_level(BDR_A, 1);
    gpio_set_level(BDR_B, 0);
}
