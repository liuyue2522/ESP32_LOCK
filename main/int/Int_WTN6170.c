#include "Int_WTN6170.h"

// 1.初始化方法
void Int_WTN6170_Init(void)
{
    // 初始化IO引脚-P9
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE, // 不需要外部中断
        .mode = GPIO_MODE_OUTPUT,       // 输出模式
        .pin_bit_mask = (1 << WTN_DATA),// 设置引脚掩码
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // 不需要下拉
        .pull_up_en = GPIO_PULLUP_DISABLE,     // 不需要上拉
    };

    // 初始化IO引脚
    gpio_config(&io_conf);

    // 延迟100ms，确保WTN6170模块上电完成
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // P9拉高，WTN6170模块进入空闲状态
    WTN_DATA_H;
}

// 2.MCU给WTN6170发送数据
void Int_WTN6170_SendData(uint8_t byte)
{
    // 1.拉低P9，准备发送数据
    WTN_DATA_L;
    vTaskDelay(5 / portTICK_PERIOD_MS);

    // 2.发送数据: 从最低位开始发送
    for (int i = 0; i < 8; i++)
    {
        // 当前位为1
        if (byte & 0x01)
        {
            // P9拉高
            WTN_DATA_H;
            usleep(600);
            // P9拉低
            WTN_DATA_L;
            usleep(200);
        }
        else // 当前位0
        {
            WTN_DATA_H;
            usleep(200);
            WTN_DATA_L;
            usleep(600);
        }

        // 右移一位，准备发送下一位
        byte >>= 1;
    }

    // 3.发送完毕，拉高P9,进入空闲状态
    WTN_DATA_H;
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
