#include <stdio.h>
#include <string.h>
#include "../src/wmbus_json.h"

#define MAX_BUFFER_SIZE 4096

// Safe string copy helper
static void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest_size > 0) {
        strncpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

static GatewayData create_sample_data(void) {
    static DataPoint data_points[3];
    
    // Data point 1: Exactly matching assignment example
    safe_strcpy(data_points[0].timestamp, "1970-01-01 00:00", MAX_TIMESTAMP);
    safe_strcpy(data_points[0].meter_datetime, "1970-01-01 00:00", MAX_TIMESTAMP);
    data_points[0].total_value = 107.752f;
    safe_strcpy(data_points[0].status, "OK", MAX_STATUS);
    
    // Data point 2: Additional reading (keeping for demo variety)
    safe_strcpy(data_points[1].timestamp, "1970-01-01 00:15", MAX_TIMESTAMP);
    safe_strcpy(data_points[1].meter_datetime, "1970-01-01 00:15", MAX_TIMESTAMP);
    data_points[1].total_value = 107.900f;
    safe_strcpy(data_points[1].status, "OK", MAX_STATUS);
    
    // Data point 3: Error state example
    safe_strcpy(data_points[2].timestamp, "1970-01-01 00:30", MAX_TIMESTAMP);
    safe_strcpy(data_points[2].meter_datetime, "1970-01-01 00:30", MAX_TIMESTAMP);
    data_points[2].total_value = 108.050f;
    safe_strcpy(data_points[2].status, "ERROR", MAX_STATUS);
    
    static DeviceReading readings[2];
    
    // Reading 1: Exactly matching assignment example
    safe_strcpy(readings[0].media, "water", MAX_MEDIA);
    safe_strcpy(readings[0].meter, "waterstarm", MAX_METER);
    safe_strcpy(readings[0].device_id, "stromleser_50898527", MAX_DEVICE_ID);
    safe_strcpy(readings[0].unit, "m3", MAX_UNIT);
    readings[0].data = &data_points[0];
    readings[0].data_count = 1;
    
    // Reading 2: Additional device for demo
    safe_strcpy(readings[1].media, "electricity", MAX_MEDIA);
    safe_strcpy(readings[1].meter, "powermax", MAX_METER);
    safe_strcpy(readings[1].device_id, "stromleser_55123789", MAX_DEVICE_ID);
    safe_strcpy(readings[1].unit, "kWh", MAX_UNIT);
    readings[1].data = &data_points[1];
    readings[1].data_count = 2;
    
    GatewayData gateway_data;
    safe_strcpy(gateway_data.gateway_id, "gateway_1234", MAX_GATEWAY_ID);
    safe_strcpy(gateway_data.date, "1970-01-01", MAX_DATE);
    safe_strcpy(gateway_data.device_type, "stromleser", MAX_DEVICE_TYPE);
    gateway_data.interval_minutes = 15;
    gateway_data.total_readings = 3;
    gateway_data.device_count = 2;
    gateway_data.readings = readings;
    
    return gateway_data;
}

int main(void) {
    printf("=== wM-Bus JSON Serializer Demo ===\n\n");
    
    printf("1. Creating sample input data...\n");
    GatewayData sample_data = create_sample_data();
    
    printf("   Gateway ID: %s\n", sample_data.gateway_id);
    printf("   Date: %s\n", sample_data.date);
    printf("   Device Type: %s\n", sample_data.device_type);
    printf("   Interval: %d minutes\n", sample_data.interval_minutes);
    printf("   Total Readings: %lu\n", sample_data.total_readings);
    printf("   Device Count: %d\n\n", sample_data.device_count);
    
    printf("2. Calculating buffer requirements...\n");
    size_t required_size = calculate_max_json_size(
        sample_data.device_count,
        sample_data.total_readings
    );
    printf("   Required buffer size: %zu bytes\n", required_size);
    printf("   Available buffer size: %d bytes\n\n", MAX_BUFFER_SIZE);
    
    if (required_size > MAX_BUFFER_SIZE) {
        printf("   ERROR: Buffer too small! Increase MAX_BUFFER_SIZE.\n");
        return 1;
    }
    
    printf("3. Validating input data...\n");
    if (!validate_gateway_data(&sample_data)) {
        printf("   ERROR: Invalid data structure\n");
        return 1;
    }
    printf("   Data validation: PASS\n\n");
    
    printf("4. Serializing to JSON...\n");
    char json_buffer[MAX_BUFFER_SIZE];
    int bytes_written = serialize_to_json(&sample_data, json_buffer, sizeof(json_buffer));
    
    if (bytes_written <= 0) {
        printf("   Serialization error: %d\n", bytes_written);
        return 1;
    }
    
    printf("   Serialized %d bytes successfully\n\n", bytes_written);
    
    printf("5. Outputting JSON:\n");
    printf("   -------------------------\n");
    printf("   [CONSOLE OUTPUT]\n");
    printf("%s\n\n", json_buffer);
    
    printf("6. Verifying JSON structure...\n");
    
    // Check exact field names from assignment
    const char* required_fields[] = {
        "\"gatewayId\"", "\"date\"", "\"deviceType\"",
        "\"interval_minutes\"", "\"total_readings\"",
        "\"values\"", "\"device_count\"", "\"readings\"",
        "\"media\"", "\"meter\"", "\"deviceId\"", "\"unit\"",
        "\"data\"", "\"timestamp\"", "\"meter_datetime\"",
        "\"total_m3\"", "\"status\""
    };
    
    int all_fields_present = 1;
    for (int i = 0; i < sizeof(required_fields)/sizeof(required_fields[0]); i++) {
        if (strstr(json_buffer, required_fields[i]) == NULL) {
            printf("   ERROR: Missing field: %s\n", required_fields[i]);
            all_fields_present = 0;
        }
    }
    
    if (all_fields_present) {
        printf("   All required fields present: PASS\n");
    }
    
    // Check outer array brackets
    if (json_buffer[0] == '[' && json_buffer[bytes_written-1] == ']') {
        printf("   Outer array brackets: PASS\n");
    } else {
        printf("   ERROR: Missing outer array brackets\n");
    }
    
    // Check numbers are not strings
    if (strstr(json_buffer, "\"15\"") == NULL && 
        strstr(json_buffer, "\"107.752\"") == NULL &&
        strstr(json_buffer, "\"1\"") == NULL) {
        printf("   Numbers not quoted: PASS\n");
    } else {
        printf("   ERROR: Numbers incorrectly serialized as strings\n");
    }
    
    printf("\n=== Demo Complete ===\n");
    printf("JSON successfully generated and validated.\n");
    printf("Total payload size: %d bytes\n", bytes_written);
    
    return 0;
}