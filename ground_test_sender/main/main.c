//This is test 1 for our esp-now receiver. What it does is that it sends a custom string like "hello world" to the receiver
//esp32 and checks if it is recived via uart


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

static void esp_now_init(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));


}









void app_main(void){
    boot_banner();
    ESP_ERROR_CHECK(nvs_flash_init());  

}