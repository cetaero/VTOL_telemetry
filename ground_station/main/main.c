/*Code has been divided into multiple tasks. The task description is given below each task calls.
Please read them and write the code accordingly.*/


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "boot_banner.h" //added boot banner
#include "mavlink/common/mavlink.h" //added Mavlink C library 



QueueHandle_t esp_to_laptop;
QueueHandle_t uart_queue;

#define PACKET_SIZE 250
#define QUEUE_SLOTS 10

// USB serial console UART. On most ESP32-C3 dev boards this is UART0,
// already wired to the onboard USB-to-serial chip — no extra pins to
// configure, no uart_set_pin() needed for it.
#define LAPTOP_UART UART_NUM_0

#define ESPNOW_CHANNEL 1

// MAC address of the airside ESP32. Replace with the real address —
// read it off the airside unit at boot (esp_wifi_get_mac) and hardcode
// it here. Packets from any other sender are ignored.
static const uint8_t AIRSIDE_MAC[6] = {0xA0, 0x85, 0xE3, 0x0D, 0x84, 0x10}; //updated based on the receiver

static const char *TAG = "GROUND_RX";


// Holds packets handed from the ESP-NOW recv callback to the forwarding
// task. Fixed size so no heap allocation happens inside the callback.
typedef struct {
    uint8_t data[PACKET_SIZE];
    int len;
} espnow_pkt_t;

// Counts packets dropped because esp_to_laptop was full, so you have
// visibility during testing instead of data silently going missing.
static volatile uint32_t dropped_packets = 0;

// ----------------------------------------------------
// USB SERIAL (UART0) INITIALIZATION
// Re-installs the UART0 driver with a larger RX/TX buffer than the
// default console driver uses, so bursts of forwarded packets don't
// get dropped. Printf/ESP_LOG still work fine afterward since they
// also go out over UART0.
// ----------------------------------------------------
static void uart_init_laptop(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };


    uart_driver_install(
        LAPTOP_UART,
        256,
        256,
        0,   //modified it based on the size of uart queue
        NULL,
        0);

    uart_param_config(
        LAPTOP_UART,
        &uart_config);

    // No uart_set_pin() call — UART0 is already routed to the USB chip
    // by the board, using default pins. Re-pinning it here would break
    // the USB serial connection.
}

// ----------------------------------------------------
// ESP-NOW RECEIVE CALLBACK
// Runs in the WiFi driver's task context. Keep this fast — no printf,
// no UART writes, no blocking calls. Just validate sender and queue it.
// ----------------------------------------------------
static void espnow_recv_cb(
    const esp_now_recv_info_t *info,
    const uint8_t *data,
    int len)
{
    // Only accept packets from the known airside MAC.
    if (memcmp(info->src_addr, AIRSIDE_MAC, 6) != 0) {
        return;
    }

    if (len <= 0) {
        return;
    }

    espnow_pkt_t pkt;

    if (len > PACKET_SIZE) {
        len = PACKET_SIZE;
    }

    memcpy(pkt.data, data, len);
    pkt.len = len;

    // Don't block here — if the queue is full, drop and count it
    // rather than stall the WiFi task.
    if (xQueueSendFromISR(esp_to_laptop, &pkt, NULL) != pdTRUE) {
        dropped_packets++;
    }
}











static void espnow_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(
        esp_event_loop_create_default());

    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(
        esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(
            WIFI_MODE_STA));

    ESP_ERROR_CHECK(
        esp_wifi_start());

    ESP_ERROR_CHECK(
        esp_wifi_set_channel(
            ESPNOW_CHANNEL,
            WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(
        esp_now_init());

    ESP_ERROR_CHECK(
        esp_now_register_recv_cb(espnow_recv_cb));

    esp_now_peer_info_t peer = {0};

memcpy(peer.peer_addr, AIRSIDE_MAC, 6);
peer.channel = ESPNOW_CHANNEL;
peer.encrypt = false;
peer.ifidx = WIFI_IF_STA;

if (!esp_now_is_peer_exist(AIRSIDE_MAC))
{
    ESP_ERROR_CHECK(
        esp_now_add_peer(&peer));
}
}



void task1(void *arg)
{
    /*Task1 pulls packets received from the air unit off the queue and
      writes the raw bytes out over USB serial (UART0) to the laptop.*/
    espnow_pkt_t pkt;

    while (true) {
        if (xQueueReceive(
                esp_to_laptop,
                &pkt,
                portMAX_DELAY))
        {
            ESP_LOGI(TAG,
         "Received packet (%d bytes)",
         pkt.len);
            // Write the exact bytes received — no strlen(), no extra
            // characters appended. Stays correct once this carries real
            // MAVLink binary frames instead of plain test strings.
            uart_write_bytes(LAPTOP_UART, (const char *)pkt.data, pkt.len);
        }
    }

}

void task2(void *arg)
{
    //task2 is to recieve bytes from GCS via UART and send them to the esp-now channel

    uint8_t buff[PACKET_SIZE]; //buffer to hold the received data
   
    while(true){
        int n = uart_read_bytes(LAPTOP_UART, buff, PACKET_SIZE, pdMS_TO_TICKS(20)); //20ms waittimee till it moves on
        if(n<0){
            ESP_LOGI(TAG, "UART reading error!!");
            //n = -1 means there has been an error. if n> 0, it is the numbe of bytes read. 
            continue;
        } 
        esp_err_t rets = esp_now_send(AIRSIDE_MAC, buff, n);
        if(rets == ESP_OK){
            ESP_LOGI(TAG, "%d bytes send successfully!", n);
        } else {
            ESP_LOGE(TAG, "ESP-NOW connection was unsuccessful!");

        }

    }
}




void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
    ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
{
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}

ESP_ERROR_CHECK(ret);

    uart_init_laptop();
    esp_to_laptop = xQueueCreate(QUEUE_SLOTS, sizeof(espnow_pkt_t));

    espnow_init();

    xTaskCreate(task1, "TX_TO_LAPTOP", 2048, NULL, 5, NULL);
    xTaskCreate(task2, "RESERVED", 2048, NULL, 5, NULL);

    printf("The ground receiver has started!\n");
}