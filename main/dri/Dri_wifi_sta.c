#include "Dri_wifi_sta.h"
//打印使用TAG
static const char *TAG = "wifi station";
//WIFI重连次数
static int s_retry_num = 0;
//事件标志组句柄
static EventGroupHandle_t s_wifi_event_group;
// WIFI事件回调
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        // WIFI_STA模式初始化成功,准备连接路由器
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // 未连接:重连,重连五次,还未连上！
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{

    // 1.创建事件标志组
    s_wifi_event_group = xEventGroupCreate();

    // 2.WIFI底层传输数据网络层协议初始化:IP协议、TCP协议栈
    esp_netif_init();

    // 3.创建事件循环组:内核0创建freeRTOS任务而已
    esp_event_loop_create_default();
    // 4. 创建并注册默认的 Wi-Fi Station 网络接口
    esp_netif_create_default_wifi_sta();

    // 5.WIFI的STA模式相关参数初始化
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // 6.初始化WIFI驱动
    esp_wifi_init(&cfg);

    // WIFI_STA模式注册两个回调,可以知道设备是否连接上路由器
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,     // 连接路由器WIFI名字
            .password = EXAMPLE_ESP_WIFI_PASS, // 连接路由器密码
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    // 设置STA
    esp_wifi_set_mode(WIFI_MODE_STA);
    // 设备连接路由器信息
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    // 设备连接你家路由器
    esp_wifi_start();

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void dri_wifi_sta_init(void)
{
    // 1.初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }

    // 初始化WIFI_STA模式
    wifi_init_sta();
}