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

## Final Performance Benchmarking

The final project compares the linked-list baseline and contiguous-array implementation at four dataset sizes: 10,000, 50,000, 100,000, and 1,000,000 readings.

All tests were performed in GitHub Codespaces on Ubuntu Linux using GCC 13.3.0. The environment reported an x86-64 AMD EPYC 7763 processor with two available CPUs.

### Compilation Optimization Levels

Compile all six program variants:

```bash
for level in 0 2 3; do
    gcc -Wall -Wextra -O"$level" baseline.c -o "baseline_O$level"
    gcc -Wall -Wextra -O"$level" optimized.c -o "optimized_O$level"
done
```

Run an example:

```bash
./baseline_O2 100000
./optimized_O2 100000
```

Optimization levels:

- `-O0`: Compiler optimizations disabled.
- `-O2`: Enables many general-purpose compiler optimizations.
- `-O3`: Enables additional aggressive optimizations.

### Reproduce the Benchmarks

Python 3 is required for the benchmarking script.

After compiling the six variants, run:

```bash
python3 benchmark.py
```

The script performs one warm-up run and five measured trials for each configuration. It checks matching output across implementations, calculates median execution times, and saves:

- `benchmark_trials.csv`: All 120 individual measurements.
- `benchmark_summary.csv`: Median times and speedup ratios.

The timer includes processing-memory allocation, copying or list construction, analysis, and processing-memory cleanup. Original data generation and result printing are excluded.

### Final Benchmark Results

All reported times are median execution times in milliseconds.

| Readings | Compiler | Baseline (ms) | Optimized (ms) | Speedup |
|---|---|---:|---:|---:|
| 10,000 | -O0 | 0.459336 | 0.174365 | 2.63x |
| 10,000 | -O2 | 0.408080 | 0.122228 | 3.34x |
| 10,000 | -O3 | 0.418691 | 0.121937 | 3.43x |
| 50,000 | -O0 | 2.403670 | 0.868326 | 2.77x |
| 50,000 | -O2 | 2.007923 | 0.527464 | 3.81x |
| 50,000 | -O3 | 1.920460 | 0.531521 | 3.61x |
| 100,000 | -O0 | 5.642377 | 1.647922 | 3.42x |
| 100,000 | -O2 | 4.028630 | 1.570166 | 2.57x |
| 100,000 | -O3 | 4.558000 | 1.091194 | 4.18x |
| 1,000,000 | -O0 | 48.179313 | 17.080349 | 2.82x |
| 1,000,000 | -O2 | 45.404227 | 11.335691 | 4.01x |
| 1,000,000 | -O3 | 40.514743 | 11.249270 | 3.60x |

The contiguous-array implementation was faster in all 12 comparisons.

At 1,000,000 readings, the optimized program took 11.335691 ms with -O2 versus 45.404227 ms for the baseline, a 4.01x speedup.

The optimized program improved from 17.080349 ms at -O0 to 11.335691 ms at -O2. At this size, -O3 provided less than a 1% improvement over -O2.

These results show benefits from both the source-level memory-layout change and compiler optimization. Small differences across configurations may also reflect system variability. Cache misses were not directly measured.

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

AddressSanitizer and UndefinedBehaviorSanitizer were later run successfully in Linux Codespaces on both implementations, including 10,000 and 1,000,000 readings. No errors or leaks were reported in those runs.

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

The assembly analysis used GCC 16.2 at -O2, while the final benchmarks used GCC 13.3.0 at -O0, -O2, and -O3.

## Further Research

Possible future improvements include measuring hardware cache misses using performance counters, examining larger datasets, and testing the program on ARM hardware.

The measurements in this repository reflect the specified Codespaces environment. Execution times may vary on other systems.
