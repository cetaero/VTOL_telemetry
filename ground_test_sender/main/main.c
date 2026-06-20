//This is test 1 for our esp-now receiver. What it does is that it sends a custom string like "hello world" to the receiver
//esp32 and checks if it is recived via uart


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_now.h"

#include "boot_banner.h"

#define TAG "ESP_NOW"

const uint8_t BROADCAST_MAC[ESP_NOW_ETH_ALEN];


void app_main(void){
    boot_banner();
    ESP_ERROR_CHECK(nvs_flash_init());  

}