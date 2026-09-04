#ifndef __INT_WS2812_H__
#define __INT_WS2812_H__

#include "led_strip.h"
#include "Com_Debug.h"


// 控制灯带IO引脚
#define LED_STRIP_GPIO_PIN GPIO_NUM_6
// 灯带LED数量
#define LED_STRIP_LED_COUNT 12
// RMT外设时钟频率
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)
// RMT外设使用内存大小
#define LED_STRIP_MEMORY_BLOCK_WORDS 0
// 未使用DMA
#define LED_STRIP_USE_DMA 0


extern uint8_t black[3];
extern uint8_t white[3];
extern uint8_t red[3];
extern uint8_t green[3];
extern uint8_t blue[3];
extern uint8_t cyan[3];
extern uint8_t purple[3];

// 1.WS2812初始化方法
void Int_WS2812_Init(void);

// 2.LED某一个灯的亮
void Int_WS2812_LEDOn(uint8_t ledIndex, uint8_t colors[]);

// 3.关闭灯带
void Int_WS2812_LEDOff(void);


#endif /* __INT_WS2812_H__ */
