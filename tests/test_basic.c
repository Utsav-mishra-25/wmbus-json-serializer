#include <stdio.h>
#include <string.h>
#include "../src/wmbus_json.h"

int main(void) {
    int test_count = 0;
    int passed = 0;
    
    printf("=== Starting wM-Bus JSON Serializer Tests ===\n\n");
    
    printf("Test %d: Basic serialization... ", ++test_count);
    {
        DataPoint dp = {
            .timestamp = "2024-01-15 14:30",
            .meter_datetime = "2024-01-15 14:30",
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
            .date = "2024-01-15",
            .device_type = "stromleser",
            .interval_minutes = 15,
            .total_readings = 1,
            .device_count = 1,
            .readings = &reading
        };
        
        char buffer[2048];
        int result = serialize_to_json(&data, buffer, sizeof(buffer));
        
        if (result > 0 &&
            strstr(buffer, "gatewayId") &&
            strstr(buffer, "total_m3") &&
            strstr(buffer, "107.752") &&
            buffer[0] == '[' &&
            buffer[strlen(buffer)-1] == ']') {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL\n");
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
            printf("PASS (calculated %zu bytes)\n", size);  // FIXED: %zu not %lu
            passed++;
        } else {
            printf("FAIL\n");
        }
    }
    
    printf("Test %d: Buffer overflow check... ", ++test_count);
    {
        DataPoint dp = {.timestamp="2024-01-01", .meter_datetime="2024-01-01", .total_value=1.0, .status="OK"};
        DeviceReading reading = {.media="test", .meter="test", .device_id="test", .unit="m3", .data=&dp, .data_count=1};
        GatewayData data = {.gateway_id="test", .date="2024-01-01", .device_type="test", .interval_minutes=1, .total_readings=1, .device_count=1, .readings=&reading};
        
        char small_buffer[10];
        int result = serialize_to_json(&data, small_buffer, sizeof(small_buffer));
        
        if (result == -1) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL (got %d)\n", result);
        }
    }
    
    printf("\n=== Test Results ===\n");
    printf("Passed: %d/%d tests\n", passed, test_count);
    
    return (passed == test_count) ? 0 : 1;
}