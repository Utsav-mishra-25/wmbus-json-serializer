#ifndef WMBUS_JSON_H
#define WMBUS_JSON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_GATEWAY_ID 32
#define MAX_DATE 11
#define MAX_DEVICE_TYPE 32
#define MAX_MEDIA 16
#define MAX_METER 32
#define MAX_DEVICE_ID 64
#define MAX_UNIT 8
#define MAX_TIMESTAMP 17
#define MAX_STATUS 8

typedef struct {
    char timestamp[MAX_TIMESTAMP];
    char meter_datetime[MAX_TIMESTAMP];
    float total_value;
    char status[MAX_STATUS];
} DataPoint;

typedef struct {
    char media[MAX_MEDIA];
    char meter[MAX_METER];
    char device_id[MAX_DEVICE_ID];
    char unit[MAX_UNIT];
    DataPoint* data;
    uint8_t data_count;
} DeviceReading;

typedef struct {
    char gateway_id[MAX_GATEWAY_ID];
    char date[MAX_DATE];
    char device_type[MAX_DEVICE_TYPE];
    uint16_t interval_minutes;
    uint32_t total_readings;
    uint8_t device_count;
    DeviceReading* readings;
} GatewayData;

size_t calculate_max_json_size(uint8_t device_count, uint32_t total_readings);
bool validate_gateway_data(const GatewayData* data);
int serialize_to_json(const GatewayData* data, char* buffer, size_t buffer_size);

#endif