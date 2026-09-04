#include "Int_WS2812.h"

// 灯带句柄
led_strip_handle_t led_strip;

// 灯带颜色定义
uint8_t black[3] = {0, 0, 0}; // 黑色
uint8_t white[3] = {255, 255, 255}; // 白色
uint8_t red[3] = {255, 0, 0}; // 红色
uint8_t green[3] = {0, 255, 0}; // 绿色
uint8_t blue[3] = {0, 0, 255}; // 蓝色
uint8_t cyan[3] = {0, 255, 255};    // 青色
uint8_t purple[3] = {255, 0, 255};  // 品红色


// 1.WS2812初始化方法
void Int_WS2812_Init(void)
{
    // 灯带相关配置
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN, // IO引脚编号
        .max_leds = LED_STRIP_LED_COUNT, // 灯带LED数量
        .led_model = LED_MODEL_WS2812, // 芯片型号
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // 颜色格式->GRB
        .flags = {
            .invert_out = false, // 信号是否反转
        }
    };
    
    // RMT外设配置
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // RMT外设时钟源
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT外设时钟频率：10MHz
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS, // RMT外设使用内存大小
        .flags = {
            .with_dma = LED_STRIP_USE_DMA, // 是否使用DMA
        }
    };

    // 初始化灯带
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
}

// 2.LED某一个灯的亮
void Int_WS2812_LEDOn(uint8_t ledIndex, uint8_t colors[])
{
    led_strip_set_pixel(led_strip, ledIndex, colors[0], colors[1], colors[2]); // 设置某一个灯的颜色
    led_strip_refresh(led_strip); // 刷新灯带
}

// 3.关闭灯带
void Int_WS2812_LEDOff(void)
{
    led_strip_clear(led_strip);
}

