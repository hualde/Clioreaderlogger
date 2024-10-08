#include "vin_utils.h"
#include "config.h"
#include <string.h>

void store_bytes(uint8_t *stored_bytes, int *stored_bytes_count, uint8_t *data, int start, int length) {
    for (int i = 0; i < length && *stored_bytes_count < VIN_LENGTH; i++) {
        stored_bytes[(*stored_bytes_count)++] = data[start + i];
    }
}

bool validate_vin(const char *vin, bool is_vehicle) {
    if (strlen(vin) != VIN_LENGTH) {
        return false;
    }
    
    for (int i = 0; i < VIN_LENGTH; i++) {
        if (vin[i] == '\0' || vin[i] == ' ') {
            return false;
        }
    }
    
    if (is_vehicle && strncmp(vin, "VF1", 3) != 0) {
        return false;
    }
    
    return true;
}