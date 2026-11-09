#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Structure for storing one weather sensor reading
typedef struct SensorNode {
    double temperature;
    double humidity;
    struct SensorNode *next;
} SensorNode;


 // Create a new sensor reading in memory
SensorNode *create_sensor_node(double temperature, double humidity) {
    SensorNode *newNode = malloc(sizeof(SensorNode));

    // Check if memory allocation was successful
    if (newNode == NULL) {
    fprintf(stderr, "Error: Memory allocation failed.\n");
    return NULL;
}

    newNode->temperature = temperature;
    newNode->humidity = humidity;
    newNode->next = NULL;

    return newNode;
}



 // Add a sensor reading to the end of the linked list
void append_sensor_node(SensorNode **head, SensorNode **tail,
                        SensorNode *newNode) {
    if (*head == NULL) {
        *head = newNode;
        *tail = newNode;
    } else {
        (*tail)->next = newNode;
        *tail = newNode;
    }
}


 // Structure for the original sensor data
typedef struct {
    double temperature;
    double humidity;
} SensorReading;

// Generate simulated weather sensor readings
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


 // Store the results of our weather analysis
typedef struct {
    double total_temperature;
    double min_temperature;
    double max_temperature;
    size_t count;
    size_t anomaly_count;
} SensorStats;

// Analyze the sensor readings in the linked list
SensorStats analyze_readings(const SensorNode *head) {
    SensorStats stats = {0};
    const SensorNode *current = head;
    double previous_temperature = 0.0;
    int has_previous = 0;

    while (current != NULL) {
        double temperature = current->temperature;
        double humidity = current->humidity;
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

        current = current->next;
    }

    return stats;
}


// Free all memory used by the linked list
void free_sensor_list(SensorNode *head) {
    SensorNode *current = head;

    while (current != NULL) {
        SensorNode *next = current->next;
        free(current);
        current = next;
    }
}

// Recursively count a limited number of linked-list nodes
size_t count_prefix_recursive(const SensorNode *node, size_t remaining) {
    if (node == NULL || remaining == 0) {
        return 0;
    }

    return 1 + count_prefix_recursive(node->next, remaining - 1);
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
        fprintf(stderr, "Usage: baseline [number_of_readings]\n");
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

    // Generate the test data before starting the timer
    SensorReading *readings = malloc(count * sizeof(SensorReading));

    if (readings == NULL) {
        fprintf(stderr, "Error: Could not allocate test data.\n");
        return EXIT_FAILURE;
    }

    generate_readings(readings, count);

    SensorNode *head = NULL;
    SensorNode *tail = NULL;

    double start = current_time_seconds();

    // Build the linked list
    for (size_t i = 0; i < count; i++) {
    SensorNode *node = create_sensor_node(
        readings[i].temperature,
        readings[i].humidity
    );

    // Free allocated memory if creating a node fails
    if (node == NULL) {
        free_sensor_list(head);
        free(readings);
        return EXIT_FAILURE;
    }

    append_sensor_node(&head, &tail, node);
}

    // Verify the first 16 nodes using recursion
    size_t nodes_to_check = count < 16 ? count : 16;

    if (count_prefix_recursive(head, nodes_to_check) != nodes_to_check) {
        fprintf(stderr, "Error: Linked list verification failed.\n");
        free_sensor_list(head);
        free(readings);
        return EXIT_FAILURE;
    }

    // Process the readings
    SensorStats stats = analyze_readings(head);

    // Release linked-list memory
    free_sensor_list(head);

    double elapsed = current_time_seconds() - start;

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
    printf("Node allocations: %zu\n", count);
    printf("Execution time: %.6f ms\n", elapsed * 1000.0);

    return EXIT_SUCCESS;
}