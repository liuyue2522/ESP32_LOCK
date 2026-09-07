#include "App_Main.h"

// 时基
#define MY_TIME_BASE (20 / portTICK_PERIOD_MS)
// 超时时间，延时3秒
int time_out = 3000 / MY_TIME_BASE;
// 计数器
uint32_t my_timer = 0;


// 收集用户输入内容缓冲区
char first_buffers[BUFFER_SIZE];
char second_buffers[BUFFER_SIZE];

// 任务2句柄
extern TaskHandle_t task2;

/******************************************************************************************/

// 清空缓冲区
static void App_Main_ClearBuffers(void);

// 校验管理员身份
static STATE_T App_Main_VerifyAdmin(void);
// 添加管理员
static void App_Main_AddAdmin(void);
// 删除管理员
static void App_Main_DelAdmin(void);

// 添加用户
static void App_Main_AddUser(void);
// 删除用户
static void App_Main_DelUser(void);

// 根据用户输入密码决定是否开门
static void App_Main_OpenDoor(char *info);

/******************************************************************************************/

// 1.项目初始化
void App_Main_Init(void)
{
    // 1.电机
    Int_BDR6120_Init();
    // 2.语音
    Int_WTN6170_Init();
    // 设置音量
   setVolume(3);
    // 3.按键
    Int_SC12B_Init();
    // 4.背光灯
    Int_WS2812_Init();
    // 5.NVS分区持久化存储用户数据
    Dri_NVS_Init();
    // 6.指纹模块
    Int_FPM383_Init();
}

// 2.获取用户输入
STATE_T App_Main_GetUserInput(char buffer[])
{
    uint8_t index = 0;
    while (1)
    {
        Key_Number_T key_number = Int_SC12B_GetKey();
        // 说明用户根本没有按下按键
        if (key_number == KEY_NULL)
        {
            // 每隔50ms+1
            my_timer++;
            // 1.超时判断,如果按键之间,相差3s,任务超时,收集用户内容行为结束!返回超时
            if (my_timer >= time_out)
            {
                Int_WS2812_LEDOff(); // 清空灯带
                // my_timer = 0; // 可以不用清零，因为下次任务开始会重新赋值
                return STATE_TIMEOUT;
            }
        }
        else
        {
            // 按下按键
            my_timer = 0;

            // 音效
            sayWithoutInt(); // 连码播放
            sayWaterDrop(); // 播放水滴声

            // 按键编号
            MY_LOGI("按键编号: %d", key_number);
            
            // 背光灯
            Int_WS2812_LEDOff(); // 清空灯带
            Int_WS2812_LEDOn(key_number, violet); // 点亮对应编号的灯

            // 按下#
            if (key_number == KEY_HASH)
            {
                Int_WS2812_LEDOff(); // 清空灯带
                return STATE_OK;
            }

            // 按下M
            if (key_number == KEY_M)
            {
                Int_WS2812_LEDOff(); // 清空灯带
                return STATE_ERROR;
            }

            // 收集用户按下的内容:密码,字符串,字符串本质字符数组,里面内容字符,不是数字!
            buffer[index++] = key_number + 48;
            if (index >= BUFFER_SIZE)
            {
                // 语音提示
                sayWithoutInt(); // 连码播放
                sayInvalid(); // 无效
                // 清空缓冲区
                App_Main_ClearBuffers();
                Int_WS2812_LEDOff(); // 清空灯带
                return STATE_FAIL;
            }
        }
        vTaskDelay(MY_TIME_BASE);
    }
}

// 3.处理用户输入内容:命令(2)、密码(6)
void App_Main_Handler(char * user_inputData)
{
    // 1.获取用户输入内容长度
    uint8_t len = strlen(user_inputData);
    // 用户输入内容小2
    if (len < 2)
    {
        // 语音提示
        sayWithoutInt();
        // 非法操作
        sayIllegalOperation();
    }
    else if (len == 2)
    {
        // 处理命令:添加管理员、删除管理员、添加普通用户密码、删除用户密码、指纹........
        if (user_inputData[0] == '0' && user_inputData[1] == '0') // 添加管理员
        {
            App_Main_AddAdmin();
        }
        else if (user_inputData[0] == '0' && user_inputData[1] == '1') // 删除管理员
        {
            App_Main_DelAdmin();
        }
        else if (user_inputData[0] == '1' && user_inputData[1] == '0') // 添加用户密码
        {
            App_Main_AddUser();
        }
        else if (user_inputData[0] == '1' && user_inputData[1] == '1') // 删除用户密码
        {
            App_Main_DelUser();
        }
        else if (user_inputData[0] == '2' && user_inputData[1] == '0') // 添加指纹
        {
            // 验证管理员身份
            STATE_T err = App_Main_VerifyAdmin();
            // 验证通过
            if (err == STATE_OK)
            {
                // 发送通知
                xTaskNotify(task2, 1, eSetValueWithOverwrite);  // eSetValueWithOverwrite: 直接覆盖写入新值
                // 通知值为1，表示添加指纹
            }
            else
            {
                sayWithoutInt();
                sayVerifyFail(); // 验证失败
            }
        }
         else if (user_inputData[0] == '2' && user_inputData[1] == '1') // 删除指纹
        {
            // 验证管理员身份
            STATE_T err = App_Main_VerifyAdmin();
            // 验证通过
            if (err == STATE_OK)
            {
                // 发送通知
                xTaskNotify(task2, 2, eSetValueWithOverwrite);
                // 通知值为2，表示删除指纹
            }
            else
            {
                sayWithoutInt();
                sayVerifyFail(); // 验证失败
            }
        }
        else
        {
            // 语音提示
            sayWithoutInt();
            // 非法操作
            sayIllegalOperation();
        }
    }
    else
    {
        // 将来输入6位密码,尝试开门!!!!
        App_Main_OpenDoor(user_inputData);
    }
}

// 4.处理用户指纹业务
void App_Main_Handler_FingerPrint(void)
{ 
    uint32_t val = 0;

    //接收通知
    //收到数据:清除数据实际,收之前、之后都清！
    xTaskNotifyWait(UINT32_MAX,UINT32_MAX,&val,0);
    MY_LOGE("收到通知:%ld", val);
}
   
/******************************************************************************************/

// 清空缓冲区
static void App_Main_ClearBuffers(void)
{
    memset(first_buffers, 0, sizeof(first_buffers));
    memset(second_buffers, 0, sizeof(second_buffers));
}

// 校验管理员身份
static STATE_T App_Main_VerifyAdmin(void)
{
    // 连码播放
    sayWithoutInt();
    // 输入管理员密码
    sayInputAdminPassword();
    // 获取用户输入内容
    STATE_T result = App_Main_GetUserInput(first_buffers);
    switch (result)
    {
    case STATE_ERROR:
        // 清除数据
        App_Main_ClearBuffers();
        return STATE_ERROR;
    case STATE_OK:
        // 先获取nvs当中存储的管理员密码
        char nvs_flash_admin_info[BUFFER_SIZE];
        size_t len = BUFFER_SIZE;
        Dri_NVS_GetValueByKey("admin", nvs_flash_admin_info, &len);
        // 校验成功
        if (strcmp(nvs_flash_admin_info, first_buffers) == 0)
        {
            App_Main_ClearBuffers();
            return STATE_OK;
        }
        App_Main_ClearBuffers();
        // 校验失败
        return STATE_FAIL;
    case STATE_TIMEOUT:
        App_Main_ClearBuffers();
        return STATE_TIMEOUT;
    default:
        App_Main_ClearBuffers();
        return STATE_OTHER;
    }
}

// 添加管理员方法
static void App_Main_AddAdmin(void)
{
    // 1.先判断管理员是否存在,如果存在就不能再添加了,给语音提示,管理员已满
    // 2.管理员账号不存在,真正添加管理员账号
    // 3.进行两次用户输入内容比较。如果两次输入内容一致,把管理员密码,写入flash当中持久化存储
    // 4.如果两次输入内容不一致,给语音提示,两次输入内容不一致添加失败
    if (Dri_NVS_FindKey("admin") != ESP_OK)
    {
        // 语音提示
        sayWithoutInt();
        sayInputAdminPassword(); // 输入管理员密码
        // 获取用户第一次输入内容
        STATE_T result1 = App_Main_GetUserInput(first_buffers);
        switch (result1)
        {
        case STATE_ERROR:
            // 语音提示
            sayWithoutInt();
            sayIllegalOperation(); // 非法操作
            break;

        case STATE_OK:
            // 二次获取用户输入内容,进行比较!一致写入FLASH,不一致不写入FLASH!
            sayWithoutInt();
            sayInputAdminPasswordAgain(); // 再次输入管理员密码
            // 获取二次输入内容
            STATE_T result2 = App_Main_GetUserInput(second_buffers);
            // 情况判断
            switch (result2)
            {
            case STATE_ERROR:
                // 语音提示
                sayWithoutInt();
                sayIllegalOperation(); // 非法操作
                break;
            case STATE_OK:
                // 比较两次输入内容是否一致
                if (strcmp(first_buffers, second_buffers) == 0)
                {
                    // 写入FLASH当中
                    esp_err_t err = Dri_NVS_SetValueByKey("admin", first_buffers);
                    // 管理员密码写入flash成功
                    if (err == ESP_OK)
                    {
                        sayWithoutInt();
                        sayAddAdmin(); // 添加管理员
                        sayWithoutInt();
                        sayAddSucc(); // 添加成功
                    }
                    else
                    {
                        sayWithoutInt();
                        sayAddAdmin(); // 添加管理员
                        sayWithoutInt();
                        sayAddFail(); // 添加失败
                    }
                }
                else
                {
                    // 1.语音提示
                    sayWithoutInt();
                    sayPasswordVerifyFail(); // 密码验证失败
                }
                break;
            case STATE_TIMEOUT:
                // 语音提示
                sayWithoutInt();
                sayRetry(); // 重试
                break;
            default:
                break;
            }
            break;

        case STATE_TIMEOUT:
            // 语音提示
            sayWithoutInt();
            sayRetry(); // 重试
            break;

        default:
            break;
        }

        // 清空缓冲区数据
        App_Main_ClearBuffers();
    }
    else
    {
        // 已有管理员
        sayWithoutInt();
        // 语音:管理员已满
        sayAdminFull();
    }
}

// 删除管理员
static void App_Main_DelAdmin(void)
{
    // 1.先判断管理员账户是否存在,不存在,语音提示!
    // 2.管理员账户存在,验证管理员身份。输入曾经设置密码
    // 3.验证成功,删除管理员账户,语音提示删除成功
    // 4.验证失败,语音提示删除失败
    if (Dri_NVS_FindKey("admin") != ESP_OK)
    {
        // 1.语音提示
        sayWithoutInt();
        sayDelAdmin(); // 删除管理员
        sayWithoutInt();
        sayDelFail(); // 删除失败
    }
    else
    {
        // 1.验证管理员身份
        STATE_T err = App_Main_VerifyAdmin();
        // 2.身份验证结果
        if (err == STATE_OK)
        {
            // 删除FLASH当中管理员信息
            // err = Dri_NVS_DeleteKey("admin");
            // 删除全部数据
            /* Dri_NVS_DeleteAll：删除flash中全部密码。   Int_FPM383_ClearAll：删除全部指纹  */
            if ((Dri_NVS_DeleteAll() == ESP_OK) && (Int_FPM383_ClearAll() == ESP_OK))
            {
                // 语音提示
                sayWithoutInt();
                sayDelAdmin(); // 删除管理员
                sayWithoutInt();
                sayDelSucc(); // 删除成功
            }
            else
            {
                // 语音提示
                sayWithoutInt();
                sayDelAdmin(); // 删除管理员
                sayWithoutInt();
                sayDelFail(); // 删除失败
            }
        }
        else
        {
            sayWithoutInt();
            sayVerifyFail(); // 验证失败
        }
    }
}

// 添加用户方法
static void App_Main_AddUser(void)
{
    // 1.先判断管理员是否存在! 不存在,不用添加用户密码!
    // 2.管理员存在,验证管理员身份,身份验证通过,添加用户信息
    // 3.身份验证失败,无法添加用户密码
    if (Dri_NVS_FindKey("admin") != ESP_OK)
    {
        sayWithoutInt();
        sayAddUser(); // 添加用户
        sayWithoutInt();
        sayAddFail(); // 添加失败
    }
    else
    {
        // 1.验证管理员身份
        STATE_T err = App_Main_VerifyAdmin();
        // 2.身份验证结果
        if (err == ESP_OK)
        {
            sayWithoutInt();
            sayInputUserPassword(); // 输入用户密码
            // 获取用户输入内容
            STATE_T result = App_Main_GetUserInput(first_buffers);
            switch (result)
            {
            case STATE_OK:
                // 持久化存储FLASH->NVS分区当中
                esp_err_t err1 = Dri_NVS_SetKey(first_buffers, 0);
                if (err1 == ESP_OK)
                {
                    sayWithoutInt();
                    sayAddUser(); // 添加用户
                    sayWithoutInt();
                    sayAddSucc(); // 添加成功
                }
                else
                {
                    sayWithoutInt();
                    sayAddUser(); // 添加用户
                    sayWithoutInt();
                    sayAddFail(); // 添加失败
                }
                break;
            case STATE_TIMEOUT:
                sayWithoutInt();
                sayRetry(); // 重试
                break;
            default:
                // 语音提示
                sayWithoutInt();
                sayIllegalOperation(); // 非法操作
                break;
            }
        }
        else
        {
            // 身份验证失败
            sayWithoutInt();
            sayVerifyFail(); // 验证失败
        }
    }
    // 清空缓冲区
    App_Main_ClearBuffers();
}

// 删除用户方法
static void App_Main_DelUser(void)
{
    // 1.先判断管理员账号是否存在,不存在,没必要删除用户信息
    // 2.管理员存在,验证身份.根据用户输入密码,决定删不删除用户信息
    if (Dri_NVS_FindKey("admin") != ESP_OK)
    {
        sayWithoutInt();
        sayDelUser(); // 删除用户
        sayWithoutInt();
        sayDelFail(); // 删除失败
    }
    else
    {
        // 1.验证管理员身份
        STATE_T err = App_Main_VerifyAdmin();
        if (err == STATE_OK)
        {
            sayWithoutInt();
            sayInputUserPassword(); // 输入用户密码
            // 获取用户输入内容
            STATE_T result = App_Main_GetUserInput(first_buffers);
            switch (result)
            {
            case STATE_OK:
                // 判断用户密码是否存在
                if (Dri_NVS_FindKey(first_buffers) == ESP_OK)
                {
                    // 从NVS分区干掉用户密码
                    err = Dri_NVS_DeleteKey(first_buffers);
                    if (err == ESP_OK)
                    {
                        sayWithoutInt();
                        sayDelUser(); // 删除用户
                        sayWithoutInt();
                        sayDelSucc(); // 删除成功
                    }
                    else
                    {
                        sayWithoutInt();
                        sayDelUser(); // 删除用户
                        sayWithoutInt();
                        sayDelFail(); // 删除失败
                    }
                }
                else
                {
                    sayWithoutInt();
                    sayDelUser(); // 删除用户
                    sayWithoutInt();
                    sayDelFail(); // 删除失败
                }
                break;
            case STATE_TIMEOUT:
                sayWithoutInt();
                sayRetry(); // 重试
                break;
            default:
                // 语音提示
                sayWithoutInt();
                sayIllegalOperation(); // 非法操作
                break;
            }

            // 清空数据
            App_Main_ClearBuffers();
        }
        else
        {
            sayWithoutInt();
            sayVerifyFail(); // 验证失败
        }
    }
}

// 根据用户输入密码决定是否开门
static void App_Main_OpenDoor(char *info)
{
    char nvs_flash_admin_info[BUFFER_SIZE];
    size_t len = BUFFER_SIZE;
    // 判断nvs分区是否有用户密码,有开门,没有语音提示
    if ((Dri_NVS_GetValueByKey("admin", nvs_flash_admin_info, &len) == ESP_OK) && (strcmp(nvs_flash_admin_info, info) == 0))
    {
        // 校验成功
        sayWithoutInt();
        sayDoorOpen(); // 开门
        Int_BDR6120_Open();
    }
    else if (Dri_NVS_FindKey(info) == ESP_OK)
    {
        sayWithoutInt();
        sayDoorOpen(); // 播放开门
        Int_BDR6120_Open();
    }
    else
    {
        sayWithoutInt();
        sayVerifyFail(); // 验证失败
        sayWithoutInt();
        sayAlarm(); // 播放警报
    }
}
