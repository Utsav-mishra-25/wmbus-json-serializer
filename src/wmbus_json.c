#include "wmbus_json.h"
#include <string.h>
#include <stdio.h>

static size_t append_str(char* buffer, size_t pos, const char* str) {
    size_t len = strlen(str);
    if (len > 0) {
        memcpy(buffer + pos, str, len);
    }
    return pos + len;
}

static size_t append_float(char* buffer, size_t pos, float value) {
    return pos + snprintf(buffer + pos, 32, "%.3f", value);
}

static size_t append_int(char* buffer, size_t pos, uint32_t value) {
    return pos + snprintf(buffer + pos, 16, "%lu", (unsigned long)value);
}

bool validate_gateway_data(const GatewayData* data) {
    if (data == NULL || data->readings == NULL) return false;
    if (data->device_count == 0) return false;
    
    for (uint8_t i = 0; i < data->device_count; i++) {
        if (data->readings[i].data_count == 0) return false;
        if (data->readings[i].data == NULL) return false;
    }
    return true;
}

size_t calculate_max_json_size(uint8_t device_count, uint32_t total_readings) {
    return 1024 + (device_count * 500) + (total_readings * 200);
}

static size_t serialize_datapoint(char* buffer, size_t pos, const DataPoint* dp) {
    pos = append_str(buffer, pos, "{\"timestamp\":\"");
    pos = append_str(buffer, pos, dp->timestamp);
    pos = append_str(buffer, pos, "\",\"meter_datetime\":\"");
    pos = append_str(buffer, pos, dp->meter_datetime);
    pos = append_str(buffer, pos, "\",\"total_m3\":");
    pos = append_float(buffer, pos, dp->total_value);
    pos = append_str(buffer, pos, ",\"status\":\"");
    pos = append_str(buffer, pos, dp->status);
    pos = append_str(buffer, pos, "\"}");
    return pos;
}

static size_t serialize_reading(char* buffer, size_t pos, const DeviceReading* reading) {
    pos = append_str(buffer, pos, "{\"media\":\"");
    pos = append_str(buffer, pos, reading->media);
    pos = append_str(buffer, pos, "\",\"meter\":\"");
    pos = append_str(buffer, pos, reading->meter);
    pos = append_str(buffer, pos, "\",\"deviceId\":\"");
    pos = append_str(buffer, pos, reading->device_id);
    pos = append_str(buffer, pos, "\",\"unit\":\"");
    pos = append_str(buffer, pos, reading->unit);
    pos = append_str(buffer, pos, "\",\"data\":[");
    
    for (uint8_t i = 0; i < reading->data_count; i++) {
        pos = serialize_datapoint(buffer, pos, &reading->data[i]);
        if (i < reading->data_count - 1) {
            pos = append_str(buffer, pos, ",");
        }
    }
    pos = append_str(buffer, pos, "]}");
    return pos;
}

int serialize_to_json(const GatewayData* data, char* buffer, size_t buffer_size) {
    if (!validate_gateway_data(data)) return -2;
    if (buffer == NULL || buffer_size == 0) return -1;
    
    size_t pos = 0;
    pos = append_str(buffer, pos, "[{");
    
    pos = append_str(buffer, pos, "\"gatewayId\":\"");
    pos = append_str(buffer, pos, data->gateway_id);
    pos = append_str(buffer, pos, "\",\"date\":\"");
    pos = append_str(buffer, pos, data->date);
    pos = append_str(buffer, pos, "\",\"deviceType\":\"");
    pos = append_str(buffer, pos, data->device_type);
    pos = append_str(buffer, pos, "\",\"interval_minutes\":");
    pos = append_int(buffer, pos, data->interval_minutes);
    pos = append_str(buffer, pos, ",\"total_readings\":");
    pos = append_int(buffer, pos, data->total_readings);
    pos = append_str(buffer, pos, ",\"values\":{\"device_count\":");
    pos = append_int(buffer, pos, data->device_count);
    pos = append_str(buffer, pos, ",\"readings\":[");
    
    for (uint8_t i = 0; i < data->device_count; i++) {
        if (pos >= buffer_size) return -1;
        pos = serialize_reading(buffer, pos, &data->readings[i]);
        if (i < data->device_count - 1) {
            pos = append_str(buffer, pos, ",");
        }
    }
    
    pos = append_str(buffer, pos, "]}}]");
    
    if (pos >= buffer_size) return -1;
    buffer[pos] = '\0';
    
    return (int)pos;
}