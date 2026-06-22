#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "boot.h"

#define TAG "GROUND"
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define CONFIG_ESPNOW_CHANNEL 1
#define PACKET_SIZE 100

typedef struct {
    uint8_t msg[PACKET_SIZE];
    size_t len;
} packet;

static void espnow_recv_cb(
    const esp_now_recv_info_t *info,
    const uint8_t *data,
    int len)
{
    packet pack;

    if (len > PACKET_SIZE) {
        len = PACKET_SIZE;
    }

    memcpy(pack.msg, data, len);
    pack.len = len;

    ESP_LOGI(TAG, "Received %d bytes", pack.len);

    printf("Data: ");

    for (int i = 0; i < pack.len; i++) {
        printf("%02X ", pack.msg[i]);
    }

    printf("\n");
}

static void start_esp_now(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(
        esp_wifi_set_channel(
            CONFIG_ESPNOW_CHANNEL,
            WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_now_init());

    esp_now_register_recv_cb(espnow_recv_cb);
}

void app_main(void)
{
    boot_banner();

    ESP_ERROR_CHECK(nvs_flash_init());

    start_esp_now();

    ESP_LOGI(TAG, "Ground Receiver Ready");
}