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

    #include "boot_banner.h"    


    QueueHandle_t pix_to_esp;
    QueueHandle_t esp_to_pix;

    #define PACKET_SIZE 100
    #define QUEUE_SLOTS 10
    #define FC_UART UART_NUM_1

    #define FC_TX_PIN GPIO_NUM_6
    #define FC_RX_PIN GPIO_NUM_7    

    #define ESPNOW_CHANNEL 1

    static const char *TAG = "AIRSIDE";

    //creating a structue for receving mavlink packet
    typedef struct {
        uint8_t msg[PACKET_SIZE]; 
        size_t len;
    } packet;



    static void uart_init_fc(void)
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
            FC_UART,
            256,
            0,
            0,
            NULL,
            0);

        uart_param_config(
            FC_UART,
            &uart_config);

        uart_set_pin(
            FC_UART,
            FC_TX_PIN,
            FC_RX_PIN,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE);
    }

    static void espnow_recv_cb(
        const esp_now_recv_info_t *info,
        const uint8_t *data,
        int len)
    {
    // char msg[PACKET_SIZE]; commented out string

        packet pack;




        if(len >= PACKET_SIZE)
        {
            len = PACKET_SIZE - 1;
        }

        memcpy(pack.msg, data, len); //edited msg with pack.msg
        pack.len = len;
        //msg[len] = '\0';

        xQueueSend(
            esp_to_pix,
            &pack, //edited msg with &pack
            0);
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

        esp_now_register_recv_cb(
            espnow_recv_cb);
    }


    void task1(void *arg){
        /*Task1 is to receive the data from pixhawk using UART and then send that same data to the ground esp32 using esp-now*/
        while(true){
            char send_data[] = "checking telem";
            xQueueSend(pix_to_esp, send_data, portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(100));

        }
    }

    void task2(void *arg)
    {
        while(true)
        {
            //char rec_data[PACKET_SIZE];
            packet pack; 

            if(xQueueReceive(
                    esp_to_pix,
                    &pack,
                    portMAX_DELAY))
            {
                ESP_LOGI(
                    TAG,
                    "Sending to Pixhawk: %d bytes sent", //%s doesnt work.
                    pack.len);

                uart_write_bytes(
                    FC_UART,
                    pack.msg, //edited rec_data with pack.msg
                    pack.len);      //edited strlen function with pack.len
            }
        }
    }



    void app_main(void)
    {
        boot_banner();
        
        ESP_ERROR_CHECK(nvs_flash_init());
        
        uart_init_fc();
        

        pix_to_esp = xQueueCreate(QUEUE_SLOTS, sizeof(packet)); //PACKET_SIZE -> sizeof(packet)
        esp_to_pix = xQueueCreate(QUEUE_SLOTS, sizeof(packet));//

        espnow_init();

        xTaskCreate(task1, "RX", 2048, NULL, 5, NULL);
        xTaskCreate(task2, "TX", 2048, NULL, 5, NULL);

        printf("The telemetry system has started!");
    }



