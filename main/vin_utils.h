#ifndef VIN_UTILS_H
#define VIN_UTILS_H

#include <stdbool.h>
#include <stdint.h>

void store_bytes(uint8_t *stored_bytes, int *stored_bytes_count, uint8_t *data, int start, int length);
bool validate_vin(const char *vin, bool is_vehicle);

#endif // VIN_UTILS_H