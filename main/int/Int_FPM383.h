#ifndef __INT_FPM383_H__
#define __INT_FPM383_H__

#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"
#include "Com_Debug.h"
#include "Com_Config.h"

#define FPM383_UART_RX GPIO_NUM_20
#define FPM383_UART_TX GPIO_NUM_21
#define FPM383_OUT GPIO_NUM_10
#define FPM383_EN GPIO_NUM_8

// 缓冲区大小
#define RX_BUF_SIZE 128


// 1.初始化方法
void Int_FPM383_Init(void);

// 2.获取FPM383序列号
void Int_FPM383_SerialNumber(void);

// 3.FPM383进入休眠模式->低功耗
void Int_FPM383_Sleep(void);

// 4.获取指纹库中还未注册ID的最小值
uint8_t Int_FPM383_GetMinID(void);

// 5.添加（自动注册模板）门锁用户指纹功能
STATE_T Int_FPM383_AddUserFingerprint(uint8_t id);

// 6.取消 自动注册模板 和 自动验证 指纹功能
void Int_FPM383_Cancel(void);

// 7.自动验证指纹（开门和删除时都需要）：如果验证成功，返回用户ID
STATE_T Int_FPM383_VerifyFingerprint(uint8_t *id);

// 8.删除用户某一个指纹
STATE_T Int_FPM383_DeleteUserFingerprint(uint8_t id);

// 9.清空FPM383指纹库（删除所有指纹）
STATE_T Int_FPM383_ClearAll(void);

#endif /* __INT_FPM383_H__ */
