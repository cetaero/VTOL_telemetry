/*Code has been divided into multiple tasks. The task description is given below each task calls.
Please read them and write the code accordingly.*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "boot_banner.h"
#include "packet.h"


QueueHandle_t pix_to_esp;
QueueHandle_t esp_to_pix;

#define PACKET_SIZE 100
#define QUEUE_SLOTS 10


void task1(void *arg){
    /*Task1 is to receive the data from pixhawk using UART and then send that same data to the ground esp32 using esp-now*/
    while(true){
        char send_data[PACKET_SIZE] = "checking telem";
        xQueueSend(pix_to_esp, send_data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));

    }
}

void task2(void *arg){
    /*Task2 is to receive the incoming data from the ground esp32 via espnow and send it to pixhawk via uart*/
    while(true){
        char rec_data[100];
        xQueueReceive(esp_to_pix, rec_data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}

void app_main(void){
    pix_to_esp = xQueueCreate(QUEUE_SLOTS, PACKET_SIZE);
    esp_to_pix = xQueueCreate(QUEUE_SLOTS, PACKET_SIZE);
    xTaskCreate(task1, "RX", 2048, NULL, 5,NULL);
    xTaskCreate(task2, "TX", 2048, NULL, 5, NULL);

    printf("The telemetry system has starter!");

}

