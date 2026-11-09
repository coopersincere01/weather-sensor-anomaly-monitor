# Weather Sensor Monitoring and Anomaly Detection System

## Project Overview

For this project, I created a C program that processes simulated weather sensor readings and identifies unusual temperature and humidity values. I wanted to explore how memory management, data structures, and computer hardware affect program performance while connecting the project to my interest in cybersecurity.

The program calculates average, minimum, and maximum temperatures and identifies readings that may indicate inaccurate data, equipment problems, or possible tampering.

The project contains two implementations:

- **Baseline (baseline.c):** Uses a singly linked list and allocates memory separately for each sensor reading.
- **Optimized (optimized.c):** Uses a contiguous array to store sensor readings, reducing the number of memory allocations and allowing sequential memory access.

Both versions use identical simulated readings and perform the same calculations.

## Project Files

- `baseline.c` - Baseline linked-list implementation
- `optimized.c` - Optimized array-based implementation
- `README.md` - Project documentation
- `.gitignore` - Excludes unnecessary files from version control

## Requirements

- GCC compiler with C support
- Windows or Linux operating system
- Terminal or command-line interface
- Visual Studio Code or GitHub Codespaces (optional)

The programs use high-resolution timing functions appropriate for their operating system:

- Windows: QueryPerformanceCounter()
- Linux: clock_gettime() with CLOCK_MONOTONIC

The final performance benchmarks were completed using GitHub Codespaces on Ubuntu Linux with GCC 13.3.0.

## Compilation Instructions

### Windows

Open a terminal in the project folder.

Compile the baseline:

```powershell
gcc -Wall -Wextra baseline.c -o baseline
```

Compile the optimized version:

```powershell
gcc -Wall -Wextra optimized.c -o optimized
```

Run the programs:

```powershell
.\baseline.exe 10000
.\optimized.exe 10000
```

### Linux / GitHub Codespaces

Open the terminal in the project folder.

Compile the baseline:

```bash
gcc -Wall -Wextra baseline.c -o baseline
```

Compile the optimized version:

```bash
gcc -Wall -Wextra optimized.c -o optimized
```

Run the programs:

```bash
./baseline 10000
./optimized 10000
```

Both implementations should compile without warnings or errors.

## Running the Programs

The programs accept a number representing how many simulated weather sensor readings to process.

Supported input sizes range from 1 to 1,000,000 readings.

Example:

```bash
./baseline 100000
./optimized 100000
```

Both programs generate the same readings and report the same temperature statistics and anomaly counts.

## Anomaly Detection

The program identifies unusual sensor readings using two rules:

1. A humidity reading below 0% or above 100%.
2. A temperature change greater than 20°F between consecutive readings.

The program generates simulated temperature and humidity readings, including occasional unusual values.

Each reading is counted as an anomaly only once, even if it meets more than one detection rule.

These anomalies could indicate inaccurate sensor data, equipment problems, or possible tampering. They do not automatically confirm a cybersecurity attack.

## Expected Output

Both implementations display:

- Total sensor readings processed
- Average temperature
- Minimum temperature
- Maximum temperature
- Number of unusual readings detected
- Number of processing memory allocations
- Execution time in milliseconds

For 10,000 readings, both implementations should produce these results:

| Measurement | Expected Result |
|---|---|
| Readings processed | 10,000 |
| Average temperature | 76.45°F |
| Minimum temperature | 68.00°F |
| Maximum temperature | 120.00°F |
| Unusual readings detected | 3 |

The baseline reports 10,000 node allocations, while the optimized version reports one processing-array allocation.

Execution time varies depending on the computer and testing conditions.

## Performance Benchmarking

I compared the baseline and optimized implementations using three dataset sizes:

- 10,000 sensor readings
- 100,000 sensor readings
- 1,000,000 sensor readings

Both programs were compiled using GCC with -Wall and -Wextra. No additional optimization flags were used, so GCC's default -O0 optimization level applied.

The benchmarking process included:

1. Running each program once as a warm-up.
2. Running each program five additional times per dataset size.
3. Recording the execution time of each run.
4. Calculating the median execution time.
5. Dividing the baseline median by the optimized median to calculate the speedup.
6. Confirming that both programs produced identical temperature statistics and anomaly counts.

The measured execution time includes allocating the processing data structure, analyzing the readings, and releasing the processing memory.

Generating the original test data and printing results are excluded from the timing.

### Benchmark Commands

Run each version with the following dataset sizes:

```bash
./baseline 10000
./optimized 10000

./baseline 100000
./optimized 100000

./baseline 1000000
./optimized 1000000
```

Repeat each command five times after a warm-up run and calculate the median reported execution time.

### Final Benchmark Results

Testing environment: GitHub Codespaces, Ubuntu Linux, GCC 13.3.0.

| Sensor Readings | Baseline (ms) | Optimized (ms) | Speedup |
|---|---:|---:|---:|
| 10,000 | 0.5015 | 0.1745 | 2.87x |
| 100,000 | 4.8182 | 1.6149 | 2.98x |
| 1,000,000 | 60.4719 | 17.0911 | 3.54x |

The optimized version was faster across all three dataset sizes.

For the largest dataset, the optimized version achieved a 3.54x speedup, exceeding my original goal of making the program at least twice as fast.

The improvement likely comes from reducing separate memory allocations and accessing sensor readings sequentially in memory.

These benchmarks measure execution time and do not directly measure CPU cache misses.

## Memory Management

### Baseline Implementation

The baseline uses malloc() to allocate memory for each linked-list node.

Each node contains:

- Temperature
- Humidity
- A pointer to the next node

After processing the readings, the free_sensor_list() function releases every node.

The baseline also includes allocation failure checks and cleanup of previously allocated memory.

A limited recursive function verifies the first 16 linked-list nodes without recursively processing the entire dataset.

### Optimized Implementation

The optimized program uses malloc() to allocate one contiguous processing array.

This reduces the number of separate processing allocations and eliminates the need for next-node pointers.

After analysis, the processing array is released using free().

Both programs also allocate and free a separate array containing their original generated test data.

### Memory Verification

Both programs were checked using GCC with:

```bash
gcc -Wall -Wextra -fanalyzer baseline.c -o baseline_check
gcc -Wall -Wextra -fanalyzer optimized.c -o optimized_check
```

These static-analysis checks completed without warnings in the original Windows development environment.

AddressSanitizer was also attempted, but the Windows GCC installation did not include the required library.

Static analysis did not identify memory problems, although it cannot guarantee that every possible memory issue has been detected.

## Assembly Analysis

I used Compiler Explorer to compare the analyze_readings() function from both implementations.

The comparison used:

- Compiler: x86-64 GCC 16.2
- Optimization level: -O2

The baseline assembly uses a memory load to retrieve the pointer to the next linked-list node:

```asm
mov rsi, QWORD PTR [rsi+16]
```

The optimized implementation uses sequential array access, including:

```asm
movsd xmm0, QWORD PTR [rsi+16]
movsd xmm5, QWORD PTR [rsi+24]
add rsi, 16
```

The optimized version advances through consecutive 16-byte sensor records without loading a separate next-node pointer.

This predictable access pattern can improve spatial locality and help the CPU make better use of its cache.

The assembly analysis used -O2, while the reported execution-time benchmarks used the default -O0 setting.

## Future Improvements

For the final version of this project, I plan to:

- Compare compilation results using -O0, -O2, and -O3.
- Investigate additional ways to improve memory layout.
- Examine how CPU caches and memory access patterns affect execution time.
- Expand the assembly analysis and architectural discussion.
- Continue testing both implementations to confirm that optimization does not change the results.

The goal is to improve the program's performance while maintaining accurate sensor analysis and proper memory management.
