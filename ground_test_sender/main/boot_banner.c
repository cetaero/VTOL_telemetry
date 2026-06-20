#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_err.h"



static const char *TAG = "ESP_NOW";

void boot_banner(void){

    
    printf("======================\n");
    printf("VTOL TELEMETRY SYSTEM\n");
    printf("AeroCET VTOL Electronics and Communication subsystem\n");
    printf("======================\n");
    

    /* Print chip information */
    esp_chip_info_t chip_info;
    
    uint8_t mac_buffer[6];
    


    esp_chip_info(&chip_info);
    printf("====CHIP INFO====");
    ESP_LOGI(TAG, "Model: %s", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG,"Cores: %d", chip_info.cores);
    
    esp_err_t ret = esp_read_mac(mac_buffer, ESP_MAC_WIFI_STA);
    if(ret == ESP_OK){
        ESP_LOGI(TAG, "Successfully retrieved MAC ADDRESS");
        ESP_LOGI(TAG, "%02X : %02X : %02X : %02X : %02X : %02X" , mac_buffer[0], mac_buffer[1], mac_buffer[2], mac_buffer[3], mac_buffer[4],mac_buffer[5]);
        ESP_LOGI(TAG, "SYSTEM READY!!!");
    
    } else {
        ESP_LOGI(TAG, "MAC ADDRESS RETRIEVAL UNSUCCESSFUL");
    }