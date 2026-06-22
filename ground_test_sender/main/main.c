//We are going to apply three tests to our receiver esp now and check if these work.


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
#include "common/mavlink.h"



#include "boot_banner.h"

#define TAG "ESP_NOW"
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define CONFIG_ESPNOW_CHANNEL 1

#define START_TEST 1

static uint8_t RECEIVER_MAC[ESP_NOW_ETH_ALEN] = {0xA0, 0x85, 0xE3, 0x0D, 0x84, 0x10}; //copied the MAC address from receiver side

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
    memcpy(peer.peer_addr, RECEIVER_MAC, ESP_NOW_ETH_ALEN);
    peer.channel = CONFIG_ESPNOW_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
}

static void test1(void *arg){
    //what test1 basically does is that it sends a custom string like "hello world" to the receiver esp32
    static const char packet[] = "hello world";
    const size_t length = strlen(packet);   

    while(true){
        esp_err_t ret = esp_now_send(RECEIVER_MAC, (const uint8_t *)packet, length);
        if(ret != ESP_OK){
            ESP_LOGE(TAG, "ESP NOW FAILED!. ERROR: %s", esp_err_to_name(ret));

        } else {
            ESP_LOGI(TAG, "Successfully Sent bytes!");

        }
        vTaskDelay(pdMS_TO_TICKS(1000));


    }
}

static void test2(void *arg){
    //so basically test 2 is sending mavlink data to airside receeiver esp32 to check if it is able to capture mavlink string
    mavlink_message_t message;
    uint8_t buff[MAVLINK_MAX_PACKET_LEN];

    while(true){
        /*static inline uint16_t mavlink_msg_heartbeat_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
        uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status)*/

        /*Careful! Mavlink V1 format is totally different form V2 format. Above one uses V2 format.*/
     /*
    * @brief Pack a heartbeat message
     * @param system_id ID of this system
    * @param component_id ID of this component (e.g. 200 for IMU)
    * @param msg The MAVLink message to compress the data into
    *
    * @param type  Vehicle or component type. For a flight controller component the vehicle type (quadrotor, helicopter, etc.). For other components the component type (e.g. camera, gimbal, etc.). This should be used in preference to component id for identifying the component type.
    * @param autopilot  Autopilot type / class. Use MAV_AUTOPILOT_INVALID for components that are not flight controllers.
    * @param base_mode  System mode bitmap.
    * @param custom_mode  A bitfield for use for autopilot-specific flags
    * @param system_status  System status flag.
    * @return length of the message in bytes (excluding serial stream start sign)
 */
        
        mavlink_msg_heartbeat_pack(
            1, //system id
            1, //component id
            &message,
            MAV_TYPE_VTOL_TILTROTOR, //for tiltrotor VTOL, this was the MAVTYPE present but I dont know if there is one another
            //specifcally for three motor tiltrotor
            MAV_AUTOPILOT_GENERIC, //Generic autopilot, full support for everything
            MAV_MODE_FLAG_GUIDED_ENABLED, // guided mode enabled, system flies waypoints / mission items. I think, it shouldnt be
            //used for actual heartbeat but Ig fine for testing
            0, //custom_mode. By default, the 32-bit custom_mode field initializes to 0.
            // ArduPilot (Copter): Mode 0 is explicitly mapped to STABILIZE.
            MAV_STATE_BOOT//SYstem is booting up. Again, heartbeat might not be this.
);

    uint8_t length1 = mavlink_msg_to_send_buffer(buff, &message); //heartbeat packet is 10 header + 9 payload + 2 checksum 
    // = 21 so uint8_t should be able to cover it.   

    esp_err_t ret1 = esp_now_send(RECEIVER_MAC,buff,length1);
    if (ret1 != ESP_OK){
        ESP_LOGE(TAG, "ESP-NOW FAILED! ERROR: %s", esp_err_to_name(ret1));
    } else {
        ESP_LOGI(TAG, "successfully send heartbeat message!");

    }


    vTaskDelay(pdMS_TO_TICKS(1000));



    }
}

void app_main(void){
    boot_banner();
    ESP_ERROR_CHECK(nvs_flash_init());  
    start_esp_now();

    #if START_TEST == 1
        xTaskCreate(test1, "test1", 4096, NULL, 5, NULL); //removed test1() as it creates an infinite loop before task create
    #else
        xTaskCreate(test2, "test2", 4096, NULL, 5, NULL);
    #endif



}