#ifndef CONFIG_H
#define CONFIG_H

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define TAG "TWAI_EXAMPLE"
#define TARGET_ID_1 0x765
#define TARGET_ID_2 0x762
#define TX_GPIO_NUM 18
#define RX_GPIO_NUM 19
#define RX_QUEUE_SIZE 64
#define VIN_LENGTH 17

#define VIN_VEHICLE_BIT BIT0
#define VIN_COLUMN_BIT BIT1
#define STOP_TASKS_BIT BIT2

extern EventGroupHandle_t vin_event_group;
extern char vin_vehiculo[VIN_LENGTH + 1];
extern char vin_columna[VIN_LENGTH + 1];

#endif // CONFIG_H