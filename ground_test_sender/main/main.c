//We are going to apply three tests to our receiver esp now and check if these work.


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_netif.h"

#include "boot_banner.h"

#define TAG "ESP_NOW"
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define CONFIG_ESPNOW_CHANNEL 1
static uint8_t BROADCAST_MAC[ESP_NOW_ETH_ALEN] = {0xA0, 0x85, 0xE3, 0x0D, 0x84, 0x10}; //copied the MAC address from receiver side

static void start_esp_now(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_now_init());

    esp_now_peer_info_t peer = {0};
    memset(&peer, 0, sizeof(esp_now_peer_info_t));
    memcpy(peer.peer_addr, BROADCAST_MAC, ESP_NOW_ETH_ALEN);
    peer.channel = CONFIG_ESPNOW_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(peer.peer_addr));
}

static test1(void){
    //what test1 basically does is that it sends a custom string like "hello world" to the receiver esp32
    static const char packet[] = "hello world";
    const size_t length = strlen(packet);   

    while(true){
        esp_err_t ret = esp_now_send(BROADCAST_MAC, (const uint8_t *)packet, length);
        if(ret != "ESP_OK"){
            ESP_LOGE(TAG, "ESP NOW FAILED!. ERROR: %s", esp_err_to_name(ret));

        } else {
            ESP_LOGI(TAG, "Successfully Sent bytes!");

        }
        vTaskDelay(pdMS_TO_TICKS(1000));


    }
}

void app_main(void){
    boot_banner();
    ESP_ERROR_CHECK(nvs_flash_init());  
    start_esp_now();
    xTaskCreate(test1, "test1", 4096, NULL, 5, NULL); //removed test1() as it creates an infinite loop before task create


}