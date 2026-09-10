#include "Int_SC12B.h"
// static char *TAG = "SC12B";

// 表示按键是否按下:0没有按下 1按下
static uint8_t key_press = 0;

// 向 从设备寄存器 写入一个字节数据
static void Int_SC12B_WriteRegisterByte(uint8_t sc12b_regisrer_addr, uint8_t data);
// 从 从设备 读取寄存器一个字节数据
static uint8_t Int_SC12B_ReadRegisterByte(uint8_t sc12b_regisrer_addr);

// p0引脚外部中断服务程序
void key_callBack(void *arg)
{
    // ESP_LOGE ESP_LOGW ESP_LOGI 不能在中断内部使用!

    // ESP_DRAM_LOGE 可以在中断内部使用
    // ESP_DRAM_LOGE(TAG, "key_callBack");
    key_press = 1;
}

// 1.初始化方法
void Int_SC12B_Init(void)
{
    // 1.初始化I2C总线
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,              // 主机模式
        .sda_io_num = SC_SDA,                 // SDA引脚
        .scl_io_num = SC_SCL,                 // SCL引脚
        .sda_pullup_en = GPIO_PULLUP_ENABLE,  // SDA上拉电阻
        .scl_pullup_en = GPIO_PULLUP_ENABLE,  // SCL上拉电阻
        .master.clk_speed = I2C_MASTER_FREQ_HZ // I2C时钟频率
    };
    // 简单来说，它根据 conf 结构体里定义好的参数（如工作模式、引脚、速度等），对I2C硬件进行设置，是使用I2C总线的第一步。
    i2c_param_config(I2C_NUM_0, &conf);
    // 将I2C总线驱动安装到系统中，使系统能够使用I2C总线进行通信。
    i2c_driver_install(I2C_NUM_0, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);


    // 2.P0引脚添加外部中断,检测到上升沿触发外部中断
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE, // 上升沿触发中断
        .mode = GPIO_MODE_INPUT,        // 输入模式
        .pin_bit_mask = (1 << SC_INT),  // 设置引脚掩码,P0引脚
        .pull_up_en = GPIO_PULLUP_DISABLE, // 不使用上拉
        .pull_down_en = GPIO_PULLDOWN_ENABLE // 需要下拉电阻
    };
    // 初始化IO引脚
    gpio_config(&io_conf);
    // 设置中断优先级
    gpio_install_isr_service(0);
    // 注册中断服务程序
    gpio_isr_handler_add(SC_INT, key_callBack, (void *)SC_INT);


    // 3.上电复位后，芯片需要300ms进行初始化，计算感应管脚的环境电容，然后才能正常工作。
    vTaskDelay(300);

    // 4.修改按键灵敏度,稍微降低一些
    Int_SC12B_WriteRegisterByte(0x00, 0x04); // 修改 CIN4按键 灵敏度
    /* CIN4 单独设置灵敏度是可以把这个按键当做接近感应电极来用，或者隔空唤醒功能，如果
        用作普通按键，把SENCH0[7:0]设成和SENCOM[7:0]一样就可以了 */
    Int_SC12B_WriteRegisterByte(0x01, 0x04); // 修改 其余通道按键 灵敏度


    MY_LOGI("SC12B初始化完成");
}

// 向从设备寄存器写入一个字节数据
static void Int_SC12B_WriteRegisterByte(uint8_t sc12b_regisrer_addr, uint8_t data)
{
    uint8_t write_buf[2] = {sc12b_regisrer_addr, data};
    i2c_master_write_to_device(I2C_NUM_0, SLAVE_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);
}

// 从 从设备 读取寄存器一个字节数据
static uint8_t Int_SC12B_ReadRegisterByte(uint8_t sc12b_regisrer_addr)
{
    uint8_t read_buf;
    i2c_master_write_read_device(I2C_NUM_0, SLAVE_ADDR, &sc12b_regisrer_addr, 1, &read_buf, 1, 1000 / portTICK_PERIOD_MS);
    return read_buf;
}

Key_Number_T Int_SC12B_GetKey(void)
{
    Key_Number_T key_num = KEY_NULL;

    if (key_press == 1) // 检测到按键按下
    {
        // 读取按键编号: 获取两个寄存器数据,拼成一个16位数据
        uint16_t key_value = (uint16_t)Int_SC12B_ReadRegisterByte(0x09) | ((uint16_t)Int_SC12B_ReadRegisterByte(0x08) << 8);
        switch (key_value)
        {
        case 0x8000:
            key_num = KEY_0;
            break;
        case 0x4000:
            key_num = KEY_1;
            break;
        case 0x2000:
            key_num = KEY_2;
            break;
        case 0x1000:
            key_num = KEY_3;
            break;
        case 0x0800:
            key_num = KEY_7;
            break;
        case 0x0400:
            key_num = KEY_5;
            break;
        case 0x0200:
            key_num = KEY_6;
            break;
        case 0x0100:
            key_num = KEY_4;
            break;
        case 0x0080:
            key_num = KEY_M;
            break;
        case 0x0040:
            key_num = KEY_8;
            break;
        case 0x0020:
            key_num = KEY_9; // #
            break;
        case 0x0010:
            key_num = KEY_HASH; // M
            break;

        default:
            key_num = KEY_NULL; // 没有按键按下
            break;
        }

        // 清除按键按下标志位
        key_press = 0;
    }
    return key_num;
}