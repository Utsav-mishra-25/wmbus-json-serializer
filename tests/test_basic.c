#include <stdio.h>
#include <string.h>
#include "../src/wmbus_json.h"

int main(void) {
    int test_count = 0;
    int passed = 0;
    
    printf("=== Starting wM-Bus JSON Serializer Tests ===\n\n");
    
    printf("Test %d: Basic serialization (assignment example)... ", ++test_count);
    {
        DataPoint dp = {
            .timestamp = "1970-01-01 00:00",
            .meter_datetime = "1970-01-01 00:00",
            .total_value = 107.752f,
            .status = "OK"
        };
        
        DeviceReading reading = {
            .media = "water",
            .meter = "waterstarm",
            .device_id = "stromleser_50898527",
            .unit = "m3",
            .data = &dp,
            .data_count = 1
        };
        
        GatewayData data = {
            .gateway_id = "gateway_1234",
            .date = "1970-01-01",
            .device_type = "stromleser",
            .interval_minutes = 15,
            .total_readings = 1,
            .device_count = 1,
            .readings = &reading
        };
        
        char buffer[2048];
        int result = serialize_to_json(&data, buffer, sizeof(buffer));
        
        // Check for exact assignment values and format
        if (result > 0 &&
            strstr(buffer, "\"gatewayId\":\"gateway_1234\"") &&
            strstr(buffer, "\"date\":\"1970-01-01\"") &&
            strstr(buffer, "\"deviceType\":\"stromleser\"") &&
            strstr(buffer, "\"interval_minutes\":15") &&        // No quotes around 15
            strstr(buffer, "\"total_readings\":1") &&           // No quotes around 1
            strstr(buffer, "\"deviceId\":\"stromleser_50898527\"") &&
            strstr(buffer, "\"total_m3\":107.752") &&           // No quotes around 107.752
            strstr(buffer, "\"total_m3\":") && !strstr(buffer, "\"total_m3\":\"") && // Not quoted
            buffer[0] == '[' &&                                 // Starts with [
            buffer[strlen(buffer)-1] == ']') {                  // Ends with ]
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL (result: %d)\n", result);
        }
    }
    
    printf("Test %d: Data validation... ", ++test_count);
    {
        GatewayData invalid_data = {.device_count = 0};
        bool valid = validate_gateway_data(&invalid_data);
        
        if (!valid) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL\n");
        }
    }
    
    printf("Test %d: Buffer size calculation... ", ++test_count);
    {
        size_t size = calculate_max_json_size(1, 1);
        if (size > 100) {
            printf("PASS (calculated %zu bytes)\n", size);  // FIXED: %zu for size_t
            passed++;
        } else {
            printf("FAIL\n");
        }
    }
    
    printf("Test %d: Buffer overflow check... ", ++test_count);
    {
        DataPoint dp = {
            .timestamp = "1970-01-01 00:00",
            .meter_datetime = "1970-01-01 00:00",
            .total_value = 1.0f,
            .status = "OK"
        };
        
        DeviceReading reading = {
            .media = "water",
            .meter = "waterstarm",
            .device_id = "stromleser_test",
            .unit = "m3",
            .data = &dp,
            .data_count = 1
        };
        
        GatewayData data = {
            .gateway_id = "gateway_test",
            .date = "1970-01-01",
            .device_type = "test",
            .interval_minutes = 1,
            .total_readings = 1,
            .device_count = 1,
            .readings = &reading
        };
        
        char small_buffer[10];  // Definitely too small
        int result = serialize_to_json(&data, small_buffer, sizeof(small_buffer));
        
        if (result == -1) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL (got %d, expected -1)\n", result);
        }
    }
    
    printf("Test %d: Invalid NULL input... ", ++test_count);
    {
        char buffer[100];
        int result = serialize_to_json(NULL, buffer, sizeof(buffer));
        
        if (result == -2) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL (got %d, expected -2)\n", result);
        }
    }
    
    printf("\n=== Test Results ===\n");
    printf("Passed: %d/%d tests\n", passed, test_count);
    
    return (passed == test_count) ? 0 : 1;
}