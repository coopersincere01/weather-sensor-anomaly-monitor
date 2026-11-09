#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Store one weather sensor reading
typedef struct {
    double temperature;
    double humidity;
} SensorReading;

// Store the results of the weather analysis
typedef struct {
    double total_temperature;
    double min_temperature;
    double max_temperature;
    size_t count;
    size_t anomaly_count;
} SensorStats;


 // Generate the same simulated weather readings as the baseline
void generate_readings(SensorReading *readings, size_t count) {
    for (size_t i = 0; i < count; i++) {
        readings[i].temperature = 68.0 + ((i / 20) % 18);
        readings[i].humidity = 35.0 + ((i / 15) % 55);

        // Simulate an unusual temperature reading
        if (i > 0 && i % 7000 == 0) {
            readings[i].temperature = 120.0;
        }

        // Simulate an invalid humidity reading
        if (i > 0 && i % 5000 == 0) {
            readings[i].humidity = 105.0;
        }
    }
}


 // Analyze sensor readings stored in a contiguous array
SensorStats analyze_readings(const SensorReading *readings, size_t count) {
    SensorStats stats = {0};
    double previous_temperature = 0.0;
    int has_previous = 0;

    for (size_t i = 0; i < count; i++) {
        double temperature = readings[i].temperature;
        double humidity = readings[i].humidity;
        int is_anomaly = 0;

        if (stats.count == 0) {
            stats.min_temperature = temperature;
            stats.max_temperature = temperature;
        }

        stats.total_temperature += temperature;

        if (temperature < stats.min_temperature) {
            stats.min_temperature = temperature;
        }

        if (temperature > stats.max_temperature) {
            stats.max_temperature = temperature;
        }

        // Check for invalid humidity
        if (humidity < 0.0 || humidity > 100.0) {
            is_anomaly = 1;
        }

        // Check for sudden temperature changes
        if (has_previous) {
            double difference = temperature - previous_temperature;

            if (difference < 0.0) {
                difference = -difference;
            }

            if (difference > 20.0) {
                is_anomaly = 1;
            }
        }

        if (is_anomaly) {
            stats.anomaly_count++;
        }

        previous_temperature = temperature;
        has_previous = 1;
        stats.count++;
    }

    return stats;
}


 // Get the current time using the Windows high-resolution timer
double current_time_seconds(void) {
#ifdef _WIN32
    // Windows high-resolution timer
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;

    if (!QueryPerformanceFrequency(&frequency) ||
        !QueryPerformanceCounter(&counter)) {
        fprintf(stderr, "Error: Could not read the timer.\n");
        exit(EXIT_FAILURE);
    }

    return (double)counter.QuadPart /
           (double)frequency.QuadPart;

#else
    // Linux monotonic high-resolution timer
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        fprintf(stderr, "Error: Could not read the timer.\n");
        exit(EXIT_FAILURE);
    }

    return (double)ts.tv_sec +
           (double)ts.tv_nsec / 1000000000.0;
#endif
}

int main(int argc, char *argv[]) {
    size_t count = 10000;

    // Allow different dataset sizes for benchmarking
    if (argc > 2) {
        fprintf(stderr, "Usage: optimized [number_of_readings]\n");
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        char *endptr;
        unsigned long requested = strtoul(argv[1], &endptr, 10);

        if (argv[1][0] == '\0' || argv[1][0] == '-' ||
            *endptr != '\0' || requested == 0 ||
            requested > 1000000UL) {
            fprintf(stderr, "Enter a number from 1 to 1000000.\n");
            return EXIT_FAILURE;
        }

        count = (size_t)requested;
    }

    // Prepare test data before starting the timer
    SensorReading *readings = malloc(count * sizeof(SensorReading));

    if (readings == NULL) {
        fprintf(stderr, "Error: Could not allocate test data.\n");
        return EXIT_FAILURE;
    }

    generate_readings(readings, count);

    double start = current_time_seconds();

    // Allocate one contiguous array for processing
    SensorReading *processed = malloc(count * sizeof(SensorReading));

    if (processed == NULL) {
        fprintf(stderr, "Error: Could not allocate sensor array.\n");
        free(readings);
        return EXIT_FAILURE;
    }

    // Copy the same readings used by the baseline
    for (size_t i = 0; i < count; i++) {
        processed[i] = readings[i];
    }

    // Analyze the readings
    SensorStats stats = analyze_readings(processed, count);

    // Release the optimized array
    free(processed);

    double elapsed = current_time_seconds() - start;

    // Release the original test data
    free(readings);

    // Display the results
    printf("Weather Sensor Monitoring Results\n");
    printf("---------------------------------\n");
    printf("Readings processed: %zu\n", stats.count);
    printf("Average temperature: %.2f F\n",
           stats.total_temperature / (double)stats.count);
    printf("Minimum temperature: %.2f F\n", stats.min_temperature);
    printf("Maximum temperature: %.2f F\n", stats.max_temperature);
    printf("Unusual readings detected: %zu\n", stats.anomaly_count);
    printf("Array allocations: 1\n");
    printf("Execution time: %.6f ms\n", elapsed * 1000.0);

    return EXIT_SUCCESS;
}


