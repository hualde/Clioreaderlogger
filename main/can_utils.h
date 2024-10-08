#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#include <stdint.h>

void send_can_frame(uint32_t id, uint8_t *data, uint8_t dlc);

#endif // CAN_UTILS_H