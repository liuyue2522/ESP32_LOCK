#include "Int_FPM383.h"

// 接收缓冲区
uint8_t rx_buffers[RX_BUF_SIZE];

// 标记:判断用户手指是否放到FPM383上面
uint8_t fpm383_flag = 0;

// p10外部中断服务程序
void Int_FPM383_CallBack(void *arg)
{
    // 检测到手指放到FPM383上面,标志位置1
    fpm383_flag = 1;
}

/********************************工具函数声明*************************************************** */
// 计算校验和
static void Int_FPM383_CheckSum(uint8_t *data, uint8_t len);

// 串口发送数据->命令
static void Int_FPM383_SendCMD(uint8_t *data, uint8_t len);

// 串口接收数据->应答数据
static void Int_FPM383_RecvData(uint32_t len, TickType_t timeout);


/********************************初始化*************************************************** */
// 1.初始化方法
void Int_FPM383_Init(void)
{
    // 1.串口相关配置
    const uart_config_t uart_config = {
        .baud_rate = 57600, // 串口波特率
        .data_bits = UART_DATA_8_BITS, // 数据位：8位
        .parity    = UART_PARITY_DISABLE, // 无校验位
        .stop_bits = UART_STOP_BITS_1, // 停止位：1位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // 没有使用硬件流工作【一般高速设备才会需要】
        .source_clk = UART_SCLK_DEFAULT, // 串口时钟源：默认
    };
    // 初始化串口
    uart_param_config(UART_NUM_1, &uart_config);
    // 安装驱动
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    // 设置串口IO引脚
    /* RTS和CTS是串口通信中用于硬件流控（Hardware Flow Control）的两个信号线 */
    /*  简单场景（低速、短数据包）：将 rts_io_num 和 cts_io_num 都设置为 -1 或 UART_PIN_NO_CHANGE，直接禁用硬件流控。*/
    /*  可靠场景（高速、长数据传输，或与 4G/GPS 模块通信）：强烈建议接上 RTS/CTS 引脚并启用硬件流控，这是保证通信稳定的关键。 */
    uart_set_pin(UART_NUM_1, FPM383_UART_TX, FPM383_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);


    // 2.控制FPM383供电的IO引脚
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,      // P8不需要外部中断
        .mode = GPIO_MODE_OUTPUT,            // 输出模式
        .pull_down_en = 0,                   // 不使用下拉
        .pull_up_en = 0,                     // 不使用上拉
        .pin_bit_mask = (1ULL << FPM383_EN), // 设置引脚
    };
    // 配置GPIO引脚
    gpio_config(&io_config);
    // FPM383_EN拉低电平，PMOS管导通，接入3V3电源开始供电
    gpio_set_level(FPM383_EN, 0);


    // 3.初始化P10引脚,添加外部中断!检测上升沿
    gpio_config_t io_config2 = {
        .intr_type = GPIO_INTR_POSEDGE,       // 上升沿触发
        .mode = GPIO_MODE_INPUT,              // 输入模式
        .pull_up_en = 0,                      // 不使用上拉
        .pull_down_en = 1,                    // 使用下拉
        .pin_bit_mask = (1ULL << FPM383_OUT), // 设置引脚
    };
    gpio_config(&io_config2);
    // 设置外部中断优先级
    gpio_install_isr_service(0);
    // 添加中断服务程序
    gpio_isr_handler_add(FPM383_OUT, Int_FPM383_CallBack, (void *)FPM383_OUT);
    // 关闭P10外部中断
    gpio_intr_disable(FPM383_OUT);


    // 4.添加延迟
    /* 复位启动时间为100ms左右，建议延时150ms */
    vTaskDelay(150);


    // 5.进入休眠状态
    Int_FPM383_Sleep();


    MY_LOGI("FPM383初始化完成");
}

/********************************发送各种命令函数封装*************************************************** */
// 2.获取FPM383序列号：    获取芯片唯一序列号PS_GetChipSN
void Int_FPM383_SerialNumber(void)
{
    // 1.发送获取序列号
    uint8_t cmd[13] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x04,             // 包长度
        0x34,                   // 指令码
        0x00,                   // 参数
        0x00, 0x39              // 校验和
    };
    /* 包长度 = 包长度至校验和（指令、参数或数据）的总字节数，包含校验和，但不包含包长度本身的字节数。 */

    // 2.发送获取序列号命令
    Int_FPM383_SendCMD(cmd, sizeof(cmd));

    // 3.接收应答数据
    Int_FPM383_RecvData(44, 2000);

    // 4.解析接收到应答数据-确认码0x00
    if (rx_buffers[9] == 0x00) // 确认码=00H 表示 OK；确认码=01H 表示收包有错；
    {
        for (uint8_t i = 0; i < 64; i++)
        {
            printf("%#02X ", rx_buffers[i]);
        }
        printf("\r\n");
    }
}

// 3.FPM383进入休眠模式->低功耗
void Int_FPM383_Sleep(void)
{
    // 1.准备休眠命令
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x03,             // 包长度
        0x33,                   // 指令码
        0x00, 0x37              // 校验和
    };

    // 进入休眠状态-多次发送
    do
    {
        // 发送休眠命令
        Int_FPM383_SendCMD(cmd, sizeof(cmd));
        // 接收应答数据
        Int_FPM383_RecvData(12, 2000);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while (rx_buffers[9] != 0x00);
    // 进入休眠状态
    MY_LOGE("进入休眠状态");
    //开启外部中断
    gpio_intr_enable(FPM383_OUT);
}

// 4.获取指纹库中还未注册ID的最小值：    读索引表PS_ReadIndexTable
uint8_t Int_FPM383_GetMinID(void)
{
    // 1.准备获取指纹库索引表
    uint8_t cmd[13] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x04,             // 包长度
        0x1f,                   // 指令码
        0x00,                   // 页码：第0页
        '\0', '\0'              // 校验和
    };
    /* 每一页有 256bit：32字节，   32*8=256 */

    // 2.计算校验和
    Int_FPM383_CheckSum(cmd, sizeof(cmd));

    // 3.发送命令
    Int_FPM383_SendCMD(cmd, sizeof(cmd));

    // 4.接受应答数据
    Int_FPM383_RecvData(44, 2000);

    // 计数器，记录查看过的指纹数量，最大100个指纹
    uint8_t count = 0;
    // 先判断确认码
    if (rx_buffers[9] == 0x00)
    {
        for (uint8_t i = 0; i < 32; i++) // 每一页：共32字节
        {
            uint8_t byte = rx_buffers[10 + i];
            // 判断字节每一位
            for (uint8_t j = 0; j < 8; j++) // 每个字节：8位
            {
                if (count >= 100) // 如果指纹库已全部查看
                {
                    return 0xff; // 返回0xff表示指纹库已满
                }
                else if (byte & 0x01) // 如果为1表示已注册
                {
                    byte >>= 1;
                    count++;
                }
                else // 如果为0表示未注册
                {
                    return 8 * i + j; // 返回最小未注册指纹ID
                }
            }
        }
    }
    // 如果100枚指纹【0-99】,都注册了,返回非0-99数字即可！
    return 0xFF;
}

// 5.添加（自动注册模板）门锁用户指纹功能
STATE_T Int_FPM383_AddUserFingerprint(uint8_t id)
{
    // 1.注册指纹命令数组
    uint8_t cmd[17] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x08,             // 包长度
        0x31,                   // 指令码
        0x00, '\0',             // 注册指纹的ID号
        0x01,                   // 录入次数
        0x00, 0x33,             // 参数:决定LED亮不亮,收集指纹数据预处理优化,相同ID可以不可以重复注册........
        '\0', '\0'              // 校验和
    };
    // 设置注册指纹的ID号
    cmd[11] = id;

    // 2. 计算校验和
    Int_FPM383_CheckSum(cmd, sizeof(cmd)); // 包长度 + 指令码 + 参数 + ID号

    // 每次注册前先取消注册四次
    Int_FPM383_Cancel();
    Int_FPM383_Cancel();
    Int_FPM383_Cancel();
    Int_FPM383_Cancel();

    // 3.发送注册指纹命令
    Int_FPM383_SendCMD(cmd, sizeof(cmd));

    // 4.接收应答数据,关键步骤的应答数据都会返回,需要接收多次!
    do
    {
        Int_FPM383_RecvData(14, 1000);

        // 判断每一次应答数据确认码是不是0
        if (rx_buffers[9] != 0x00) // 0x00:表示成功, 如果有一条返回失败,则注册失败!
        {
            return STATE_FAIL;
        }

    } while (rx_buffers[10] != 0x06); // 0x06:表示注册指纹成功,如果接收到的数据不是0x06,则继续接收

    // 注册成功
    return STATE_OK;
}

// 6.取消 自动注册模板 和 自动验证 指纹功能
void Int_FPM383_Cancel(void)
{
    // 准备发送取消注册模版指令
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x03,             // 包长度
        0x30,                   // 指令码
        '\0', '\0'              // 校验和
    };
    // 计算校验和
    Int_FPM383_CheckSum(cmd, sizeof(cmd));

    // 2.发送取消命令
    do
    {
        Int_FPM383_SendCMD(cmd, sizeof(cmd));
        Int_FPM383_RecvData(12, 2000);
    } while (rx_buffers[9] != 0x00); // 确认码=00H 表示取消设置成功
    MY_LOGE("取消注册指纹模版成功");
}

// 7.自动验证指纹（开门和删除时都需要）：如果验证成功，返回用户ID
STATE_T Int_FPM383_VerifyFingerprint(uint8_t *id)
{
    uint8_t cmd[17] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x08,             // 包长度
        0x32,                   // 指令码
        0x03,                   // 分数等级
        0xFF, 0xFF,             // ID号:0xFFFF,当前指纹与指纹库进行对比。 或者例如：0x0011,跟第17指纹进行对比
        0x00, 0x07,             // 参数:表示只需要最后验证结果!!!!
        '\0', '\0'              // 校验和
    };

    // 计算校验和
    Int_FPM383_CheckSum(cmd, sizeof(cmd));

    // 发送验证指纹命令
    Int_FPM383_SendCMD(cmd, sizeof(cmd));

    // 接收应答数据
    Int_FPM383_RecvData(17, 2000);

    // 判断确认码
    if (rx_buffers[9] == 0x00)
    {
        if (id != NULL)
        {
            // 指纹ID:两个字节,0-99,高八位没用!
            *id = rx_buffers[12];
        }
        return STATE_OK;
    }

    return STATE_FAIL;
}

// 8.删除用户某一个指纹
STATE_T Int_FPM383_DeleteUserFingerprint(uint8_t id)
{
    // 发送删除某一个指纹命令
    uint8_t cmd[16] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x07,             // 包长度
        0x0C,                   // 指令码
        0x00, '\0',             // 删除指纹ID编号
        0x00, 0x01,             // 删除指纹个数
        '\0', '\0'              // 校验和
    };
    cmd[11] = id;

    // 计算校验和
    Int_FPM383_CheckSum(cmd, sizeof(cmd));

    // 发送删除指纹命令
    Int_FPM383_SendCMD(cmd, sizeof(cmd));

    // 接收应答数据
    Int_FPM383_RecvData(12, 2000);

    // 判断确认码
    if (rx_buffers[9] == 0x00)
    {
        return STATE_OK;
    }
    return STATE_FAIL;
}

// 9.清空FPM383指纹库（删除所有指纹）
STATE_T Int_FPM383_ClearAll(void)
{
    // 删除指纹库
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标识
        0x00, 0x03,             // 包长度
        0x0D,                   // 指令码
        0x00, 0x11              // 校验和
    };

    // 发送命令
    Int_FPM383_SendCMD(cmd, sizeof(cmd));

    // 接收应答数据
    Int_FPM383_RecvData(12, 2000);

    if (rx_buffers[9] == 0x00)
    {
        return STATE_OK;
    }
    return STATE_FAIL;
}


/********************************工具函数封装*************************************************** */
// 计算校验和
/* 校验和是从 包标识 至 校验和之前 所有字节的 *数据* 之和，包含 包标识 但不包含 校验和，超出2 字节的进位忽略。 */
static void Int_FPM383_CheckSum(uint8_t *data, uint8_t len)
{
    uint16_t sum = 0;
    // 累加:包表示 + 不包含最后两个字节校验和
    for (uint8_t i = 6; i < len - 2; i++)
    {
        sum += data[i];
    }

    // 将计算完校验和结果添加进入
    data[len - 2] = (sum >> 8) & 0xFF;
    data[len - 1] = sum & 0xFF;
}

// 串口发送数据->命令
static void Int_FPM383_SendCMD(uint8_t *data, uint8_t len)
{
    // 通过串口1,TX引脚发送命令
    uart_write_bytes(UART_NUM_1, data, len);
}

// 串口接收数据->应答数据
static void Int_FPM383_RecvData(uint32_t len, TickType_t timeout)
{
    // 加入发送多个命令,接收应答数据都是放在rx_buffers缓冲区,因为每一次在手新的应答数据之前清空数据
    // 每一次清空数据里面放置0xFF,别写0
    memset(rx_buffers, 0xFF, RX_BUF_SIZE);
    // 接收FPM383响应的数据
    uart_read_bytes(UART_NUM_1, rx_buffers, len, timeout);

    // 判断接收到的数据是否正确
    uint8_t verifyByte1 = 0XEF;
    uint8_t verifyByte2 = 0X01;
    uint8_t offset = 0; // 偏移量
    for (uint8_t i = 0; i < len; i++)
    {
        if ((rx_buffers[i] == verifyByte1) && (rx_buffers[i + 1] == verifyByte2))
        {
            offset = i;
            break; // 找到正确的数据,跳出循环
        }
    }
    // 将数据偏移到正确的位置
    for (uint8_t i = 0; i < len; i++)
    {
        rx_buffers[i] = rx_buffers[i + offset];
        printf("%#02X ", rx_buffers[i]);
    }
    printf("\n");
}
