#include "can_utils.h"
#include "config.h"
#include "driver/twai.h"
#include "esp_log.h"
#include <string.h>

void send_can_frame(uint32_t id, uint8_t *data, uint8_t dlc) {
    if (xEventGroupGetBits(vin_event_group) & STOP_TASKS_BIT) {
        return;
    }

    twai_message_t message;
    message.identifier = id;
    message.extd = 0;
    message.rtr = 0;
    message.data_length_code = dlc;
    memcpy(message.data, data, dlc);

    esp_err_t result = twai_transmit(&message, pdMS_TO_TICKS(1000));
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Error al enviar trama CAN: ID=0x%03" PRIx32 " Error: %s", id, esp_err_to_name(result));
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
}