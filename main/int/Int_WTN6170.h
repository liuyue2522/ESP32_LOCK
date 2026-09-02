#ifndef INT_WTN6170_H
#define INT_WTN6170_H

#include "driver/gpio.h"
#include "esp_task.h"
#include "sys/unistd.h"
#include "Com_Debug.h"

#define WTN_DATA GPIO_NUM_9
#define WTN_DATA_H (gpio_set_level(WTN_DATA, 1)) // 拉高数据线
#define WTN_DATA_L (gpio_set_level(WTN_DATA, 0)) // 拉低数据线

// 命令宏定义
#define sayWithoutInt() Int_WTN6170_SendData(0xf3) // 连码播放
#define setVolume(x) Int_WTN6170_SendData(0xE0 + x) // 设置音量


// 语音播报宏定义
#define sayNum(x) Int_WTN6170_SendData(x + 1) // 播放数字
#define sayWaterDrop() Int_WTN6170_SendData(11) // 播放水滴
#define sayBuzzer() Int_WTN6170_SendData(12) // 播放蜂鸣器
#define sayAlarm() Int_WTN6170_SendData(13) // 播放警报
#define sayDoorBell() Int_WTN6170_SendData(14) // 播放门铃
#define sayFail() Int_WTN6170_SendData(16) // 失败音效
#define sayPassword() Int_WTN6170_SendData(19) // 播放密码
#define sayDoorOpen() Int_WTN6170_SendData(25) // 播放开门
#define sayDoorClose() Int_WTN6170_SendData(26) // 播放关门
#define sayIllegalOperation() Int_WTN6170_SendData(28) // 非法操作
#define sayVerify() Int_WTN6170_SendData(31) // 确认
#define sayFinish() Int_WTN6170_SendData(33) // 结束
#define sayPressSharp() Int_WTN6170_SendData(34) // 按下井号
#define sayDelSucc() Int_WTN6170_SendData(36) // 删除成功
#define sayDelFail() Int_WTN6170_SendData(37) // 删除失败
#define sayVerifySucc() Int_WTN6170_SendData(41) // 验证成功
#define sayVerifyFail() Int_WTN6170_SendData(42) // 验证失败
#define saySetSucc() Int_WTN6170_SendData(43) // 设置成功
#define saySetFail() Int_WTN6170_SendData(44) // 设置失败
#define sayOperateSucc() Int_WTN6170_SendData(46) // 操作成功
#define sayOperateFail() Int_WTN6170_SendData(47) // 操作失败
#define sayInvalid() Int_WTN6170_SendData(48) // 无效
#define sayAddSucc() Int_WTN6170_SendData(49) // 添加成功
#define sayAddFail() Int_WTN6170_SendData(50) // 添加失败
#define sayAddUser() Int_WTN6170_SendData(51) // 添加用户
#define sayDelUser() Int_WTN6170_SendData(52) // 删除用户
#define sayUserFull() Int_WTN6170_SendData(53) // 用户已满
#define sayAddAdmin() Int_WTN6170_SendData(54) // 添加管理员
#define sayDelAdmin() Int_WTN6170_SendData(55) // 删除管理员
#define sayAdminFull() Int_WTN6170_SendData(56) // 管理员已满
#define sayRetry() Int_WTN6170_SendData(58) // 重试
#define sayInputUserPassword() Int_WTN6170_SendData(64) // 输入用户密码
#define sayInputUserPasswordAgain() Int_WTN6170_SendData(65) // 再次输入用户密码
#define sayPasswordAddSucc() Int_WTN6170_SendData(66) // 密码添加成功
#define sayPasswordAddFail() Int_WTN6170_SendData(67) // 密码添加失败
#define sayPasswordVerifySucc() Int_WTN6170_SendData(68) // 密码验证成功
#define sayPasswordVerifyFail() Int_WTN6170_SendData(69) // 密码验证失败
#define sayInputAdminPassword() Int_WTN6170_SendData(70) // 输入管理员密码
#define sayInputAdminPasswordAgain() Int_WTN6170_SendData(71) // 再次输入管理员密码
#define sayAddUserFingerprint() Int_WTN6170_SendData(76) // 添加用户指纹
#define sayDelUserFingerprint() Int_WTN6170_SendData(77) // 删除用户指纹
#define sayPlaceFinger() Int_WTN6170_SendData(80) // 请放置手指
#define sayPlaceFingerAgain() Int_WTN6170_SendData(81) // 请再次放置手指
#define sayTakeAwayFinger() Int_WTN6170_SendData(82) // 请拿开手指
#define sayFingerprintAddSucc() Int_WTN6170_SendData(83) // 指纹添加成功
#define sayFingerprintAddFail() Int_WTN6170_SendData(84) // 指纹添加失败
#define sayFingerprintVerifySucc() Int_WTN6170_SendData(85) // 指纹验证成功
#define sayFingerprintVerifyFail() Int_WTN6170_SendData(86) // 指纹验证失败

// 1.初始化方法
void Int_WTN6170_Init(void);
// 2.MCU给WTN6170发送数据
void Int_WTN6170_SendData(uint8_t byte);

#endif // INT_WTN6170_H
