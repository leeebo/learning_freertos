/*  FreeRTOS Real Time Stats Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "TASK";
static QueueHandle_t evt_queue = NULL;

static void sender_task(void *arg)
{
    uint32_t io_num = 0; 
    ESP_LOGI(TAG, "Sending task");
    while (1) {
        ESP_LOGI(TAG, "Sending = %" PRIu32, io_num);
        xQueueSend(evt_queue, &io_num, portMAX_DELAY);
        ESP_LOGI(TAG, "Sent = %" PRIu32, io_num);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void receiver_task(void *arg)
{
    uint32_t io_num = 0;
    ESP_LOGI(TAG, "Receiver task");
    while (1) {
        if(xQueueReceive(evt_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGW(TAG, "Received = %" PRIu32, io_num);
        }
    }
}

void app_main(void)
{
    //Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
    evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //Create and start stats task
    xTaskCreatePinnedToCore(sender_task, "sender", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(receiver_task, "receiver", 4096, NULL, 6, NULL, 0);
}
