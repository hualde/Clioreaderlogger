#include "tasks.h"
#include "config.h"
#include "can_utils.h"
#include "vin_utils.h"
#include "driver/twai.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

// Variables globales externas
extern char vin_vehiculo[VIN_LENGTH + 1];
extern char vin_columna[VIN_LENGTH + 1];

// Variables locales
static uint8_t stored_bytes_vehicle[VIN_LENGTH];
static uint8_t stored_bytes_column[VIN_LENGTH];
static int stored_bytes_count_vehicle = 0;
static int stored_bytes_count_column = 0;

void empty_task_199(void *pvParameters) {
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    send_can_frame(0x199, data, 8);
    ESP_LOGI(TAG, "Tarea vacía con ID 199 ejecutada");
    vTaskDelete(NULL);
}

void communication_task(void *pvParameters) {
    // Tramas para el vehículo
    uint8_t frame1[] = {0x02, 0x10, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame2[] = {0x02, 0x21, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame3[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame4[] = {0x02, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame5[] = {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame6[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame7[] = {0x02, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame8[] = {0x02, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame9[] = {0x02, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame10[] = {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame11[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame12[] = {0x02, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame13[] = {0x02, 0x21, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame14[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame15[] = {0x02, 0x21, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame16[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Tramas para la columna
    uint8_t frame17[] = {0x02, 0x10, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame18[] = {0x02, 0x21, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame19[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame20[] = {0x02, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame21[] = {0x02, 0x21, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t frame22[] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    while (1) {
        EventBits_t bits = xEventGroupGetBits(vin_event_group);
        if (bits & STOP_TASKS_BIT) {
            ESP_LOGI(TAG, "Deteniendo la tarea de comunicacion.");
            vTaskDelete(NULL);
        }

        if (!(bits & VIN_VEHICLE_BIT)) {
            // Enviar tramas para el vehículo
            send_can_frame(0x745, frame1, 8);
            send_can_frame(0x745, frame2, 8);
            send_can_frame(0x745, frame3, 8);
            send_can_frame(0x745, frame4, 8);
            send_can_frame(0x745, frame5, 8);
            send_can_frame(0x745, frame6, 8);
            send_can_frame(0x745, frame7, 8);
            send_can_frame(0x745, frame8, 8);
            send_can_frame(0x745, frame9, 8);
            send_can_frame(0x745, frame10, 8);
            send_can_frame(0x745, frame11, 8);
            send_can_frame(0x745, frame12, 8);
            send_can_frame(0x745, frame13, 8);
            send_can_frame(0x745, frame14, 8);
            send_can_frame(0x745, frame15, 8);
            send_can_frame(0x745, frame16, 8);
        } else if (!(bits & VIN_COLUMN_BIT)) {
            // Enviar tramas para la columna
            send_can_frame(0x742, frame17, 8);
            send_can_frame(0x742, frame18, 8);
            send_can_frame(0x742, frame19, 8);
            send_can_frame(0x742, frame20, 8);
            send_can_frame(0x742, frame21, 8);
            send_can_frame(0x742, frame22, 8);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void receive_task(void *pvParameters) {
    while (1) {
        EventBits_t bits = xEventGroupGetBits(vin_event_group);
        if (bits & STOP_TASKS_BIT) {
            ESP_LOGI(TAG, "Deteniendo la tarea de recepcion.");
            vTaskDelete(NULL);
        }

        twai_message_t message;
        esp_err_t result = twai_receive(&message, pdMS_TO_TICKS(100));
        if (result == ESP_OK) {
            if (message.identifier == TARGET_ID_1 || message.identifier == TARGET_ID_2) {
                uint8_t *target_stored_bytes;
                int *target_stored_bytes_count;
                char *target_VIN;
                EventBits_t vin_bit;
                bool is_vehicle;

                if (message.identifier == TARGET_ID_1 && !(bits & VIN_VEHICLE_BIT)) {
                    target_stored_bytes = stored_bytes_vehicle;
                    target_stored_bytes_count = &stored_bytes_count_vehicle;
                    target_VIN = vin_vehiculo;
                    vin_bit = VIN_VEHICLE_BIT;
                    is_vehicle = true;
                } else if (message.identifier == TARGET_ID_2 && !(bits & VIN_COLUMN_BIT)) {
                    target_stored_bytes = stored_bytes_column;
                    target_stored_bytes_count = &stored_bytes_count_column;
                    target_VIN = vin_columna;
                    vin_bit = VIN_COLUMN_BIT;
                    is_vehicle = false;
                } else {
                    continue;
                }

                if (message.data[0] == 0x10 && message.data_length_code >= 8) {
                    store_bytes(target_stored_bytes, target_stored_bytes_count, message.data, 4, 4);
                } 
                else if (message.data[0] == 0x21 && message.data_length_code == 8) {
                    store_bytes(target_stored_bytes, target_stored_bytes_count, message.data, 1, 7);
                } 
                else if (message.data[0] == 0x22 && message.data_length_code == 8) {
                    store_bytes(target_stored_bytes, target_stored_bytes_count, message.data, 1, 6);
                }

                // Cuando se han recibido los 17 bytes del VIN
                if (*target_stored_bytes_count == VIN_LENGTH) {
                    // Convertir los bytes almacenados a caracteres ASCII
                    for (int i = 0; i < VIN_LENGTH; i++) {
                        target_VIN[i] = (char)target_stored_bytes[i];
                    }
                    target_VIN[VIN_LENGTH] = '\0'; // Añadir el carácter nulo al final

                    if (validate_vin(target_VIN, is_vehicle)) {
                        if (is_vehicle) {
                            ESP_LOGI(TAG, "VIN del vehiculo valido: %s", target_VIN);
                        } else {
                            ESP_LOGI(TAG, "VIN de la columna valido: %s", target_VIN);
                        }
                        xEventGroupSetBits(vin_event_group, vin_bit);
                    } else {
                        if (is_vehicle) {
                            ESP_LOGW(TAG, "VIN del vehiculo no valido: %s", target_VIN);
                        } else {
                            ESP_LOGW(TAG, "VIN de la columna no valido: %s", target_VIN);
                        }
                        *target_stored_bytes_count = 0; // Reiniciar para intentar de nuevo
                        continue;
                    }
                    
                    // Reiniciar el contador
                    *target_stored_bytes_count = 0;
                }
            }
        } else if (result != ESP_ERR_TIMEOUT) {
            ESP_LOGE(TAG, "Error al recibir trama CAN: %s", esp_err_to_name(result));
        }
    }
}

void set_vin_task(void *pvParameters) {
    uint8_t data1[] = {0x02, 0x10, 0xC0, 0x30, 0x30, 0x30, 0x30, 0x30};
    send_can_frame(0x742, data1, 8);

    uint8_t data2[8] = {0x10, 0x15, 0x3B, 0x81, 0, 0, 0, 0};
    memcpy(data2 + 4, vin_vehiculo, 4);
    send_can_frame(0x742, data2, 8);

    uint8_t data3[8] = {0x21, 0, 0, 0, 0, 0, 0, 0};
    memcpy(data3 + 1, vin_vehiculo + 4, 7);
    send_can_frame(0x742, data3, 8);

    uint8_t data4[8] = {0x22, 0, 0, 0, 0, 0, 0, 0};
    memcpy(data4 + 1, vin_vehiculo + 11, 6);
    data4[7] = 0x49;
    send_can_frame(0x742, data4, 8);

    uint8_t data5[] = {0x23, 0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    send_can_frame(0x742, data5, 8);

    uint8_t data6[] = {0x02, 0x10, 0xFA, 0x30, 0x30, 0x30, 0x30, 0x30};
    send_can_frame(0x742, data6, 8);

    ESP_LOGI(TAG, "VIN establecido");
    vTaskDelete(NULL);
}

void set_immo_task(void *pvParameters) {
    uint8_t data1[] = {0x02, 0x10, 0xC0, 0x08, 0x00, 0x00, 0x00, 0x08};
    send_can_frame(0x742, data1, 8);

    uint8_t data2[] = {0x02, 0x10, 0xFB, 0x08, 0x00, 0x00, 0x00, 0x08};
    send_can_frame(0x742, data2, 8);

    uint8_t data3[] = {0x02, 0x3B, 0x05, 0x08, 0x00, 0x00, 0x00, 0x08};
    send_can_frame(0x742, data3, 8);

    uint8_t data4[] = {0x02, 0x10, 0xFA, 0x0D, 0xB6, 0x01, 0xFF, 0xFF};
    send_can_frame(0x742, data4, 8);

    

    ESP_LOGI(TAG, "IMMO programado");
    vTaskDelete(NULL);
}

void clear_dtc_task(void *pvParameters) {
    uint8_t data[] = {0x03, 0x14, 0xFF, 0x00, 0xB6, 0x01, 0xFF, 0xFF};
    send_can_frame(0x742, data, 8);

    ESP_LOGI(TAG, "Senal de borrado de fallos enviada");
    vTaskDelete(NULL);
}