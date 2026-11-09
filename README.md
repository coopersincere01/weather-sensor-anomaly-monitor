# Weather Sensor Monitoring and Anomaly Detection System

## Project Overview

This project is a C program that processes simulated weather sensor readings and identifies unusual temperature and humidity values. I created it to explore how memory management and data structures affect program performance. The program calculates average, minimum, and maximum temperatures while also identifying readings that may need further investigation.

The project includes two implementations:

- **Baseline (`baseline.c`):** Uses a singly linked list and allocates memory separately for each sensor reading.
- **Optimized (`optimized.c`):** Uses a contiguous array to store sensor readings, reducing the number of memory allocations and improving how the CPU accesses the data.

Both programs perform the same calculations and use identical simulated readings so their performance can be compared fairly.

## Requirements

- Windows operating system
- GCC compiler with C support
- PowerShell or another compatible terminal
- Visual Studio Code (optional, for editing the code)

The programs use the Windows `QueryPerformanceCounter()` function to measure execution time.

## Compilation Instructions

Open a terminal inside the project folder.

Compile the baseline version:

`gcc -Wall baseline.c -o baseline`

Compile the optimized version:

`gcc -Wall optimized.c -o optimized`

Both programs should compile without warnings or errors.

## Running the Programs

Run the baseline version:

`.\baseline.exe 10000`

Run the optimized version:

`.\optimized.exe 10000`

The number represents how many simulated weather sensor readings the program will process.

The programs support input sizes from 1 to 1,000,000 readings.

## Benchmarking Instructions

To compare performance, run both programs using three different dataset sizes:

| Dataset | Baseline Command | Optimized Command |
|---|---|---|
| 10,000 | `.\baseline.exe 10000` | `.\optimized.exe 10000` |
| 100,000 | `.\baseline.exe 100000` | `.\optimized.exe 100000` |
| 1,000,000 | `.\baseline.exe 1000000` | `.\optimized.exe 1000000` |

For each dataset size:

1. Run both programs once as a warm-up.
2. Run each version five additional times.
3. Record the execution time reported by each run.
4. Calculate the median execution time for each version.
5. Divide the baseline median by the optimized median to calculate the speedup.

Both programs were initially compiled using `gcc -Wall` without additional optimization flags. Data generation and printing are excluded from the timed section. The measurements include preparing the processing data structure, analyzing the readings, and freeing the processing memory.

## Expected Output

Both programs display:

- Number of sensor readings processed
- Average temperature
- Minimum temperature
- Maximum temperature
- Number of unusual readings detected
- Number of processing memory allocations
- Execution time in milliseconds

For 10,000 readings, both programs should report:

| Measurement | Expected Result |
|---|---|
| Readings processed | 10,000 |
| Average temperature | 76.45°F |
| Minimum temperature | 68.00°F |
| Maximum temperature | 120.00°F |
| Unusual readings detected | 3 |

The baseline reports 10,000 node allocations, while the optimized version reports one processing-array allocation. Execution times vary depending on the computer and system conditions.

## Anomaly Detection

The program identifies unusual readings based on two rules:

- Humidity readings below 0% or above 100%
- Temperature changes greater than 20°F between consecutive readings

These anomalies may indicate inaccurate sensor data, equipment problems, or possible tampering. They do not automatically confirm a cybersecurity attack.

## Memory Management

The baseline uses `malloc()` to allocate individual linked-list nodes and `free()` to release them after processing. The optimized version allocates one contiguous processing array and releases it after the analysis is complete.

Both versions also allocate and release a separate array used to generate the original test data.

## Project Files

- `baseline.c` — Baseline linked-list implementation
- `optimized.c` — Optimized array implementation
- `README.md` — Project documentation
- `.gitignore` — Excludes compiled executables and other unnecessary files

## Future Improvements

For the final version, I plan to compare additional compiler optimization settings, examine assembly output, and further investigate how CPU cache behavior and memory layout affect performance.