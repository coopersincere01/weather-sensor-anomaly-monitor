import csv
import re
import statistics
import subprocess

SIZES = [10000, 50000, 100000, 1000000]
LEVELS = ["O0", "O2", "O3"]
PROGRAMS = ["baseline", "optimized"]
TRIALS = 5

FIELDS = [
    "Readings processed",
    "Average temperature",
    "Minimum temperature",
    "Maximum temperature",
    "Unusual readings detected",
]


def run_program(program, level, size):
    result = subprocess.run(
        [f"./{program}_{level}", str(size)],
        capture_output=True,
        text=True,
        check=True,
    )
    output = result.stdout

    values = []
    for field in FIELDS:
        match = re.search(
            rf"^{re.escape(field)}:\s*(.+)$",
            output,
            re.MULTILINE,
        )
        if not match:
            raise ValueError(f"Missing {field} in {program}_{level}")
        values.append(match.group(1))

    match = re.search(r"Execution time:\s*([\d.]+) ms", output)
    if not match:
        raise ValueError(f"Missing execution time in {program}_{level}")

    return tuple(values), float(match.group(1))


trials = []
summary = []

for size in SIZES:
    print(f"\nTesting {size:,} readings...")
    expected = None

    # Warm up every executable and check correctness.
    for level in LEVELS:
        for program in PROGRAMS:
            values, _ = run_program(program, level, size)
            if expected is None:
                expected = values
            elif values != expected:
                raise ValueError(
                    f"Output mismatch: {program}_{level}, size {size}"
                )

    # Interleave the measured runs.
    measurements = {}
    for trial in range(1, TRIALS + 1):
        for level in LEVELS:
            for program in PROGRAMS:
                values, elapsed = run_program(program, level, size)
                if values != expected:
                    raise ValueError(
                        f"Output mismatch: {program}_{level}, size {size}"
                    )
                key = (level, program)
                measurements.setdefault(key, []).append(elapsed)
                trials.append([
                    size, level, program, trial, elapsed
                ])

    for level in LEVELS:
        baseline = statistics.median(
            measurements[(level, "baseline")]
        )
        optimized = statistics.median(
            measurements[(level, "optimized")]
        )
        speedup = baseline / optimized
        summary.append([
            size, level, baseline, optimized, speedup
        ])
        print(
            f"  {level}: baseline={baseline:.6f} ms, "
            f"optimized={optimized:.6f} ms, "
            f"speedup={speedup:.2f}x"
        )

with open("benchmark_trials.csv", "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow([
        "input_size", "optimization", "implementation",
        "trial", "execution_ms"
    ])
    writer.writerows(trials)

with open("benchmark_summary.csv", "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow([
        "input_size", "optimization", "baseline_median_ms",
        "optimized_median_ms", "speedup"
    ])
    writer.writerows(summary)

print("\nAll output checks passed.")
print("Saved benchmark_trials.csv and benchmark_summary.csv.")
