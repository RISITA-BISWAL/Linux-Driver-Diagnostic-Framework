# Linux Device Driver Testing and Diagnostic Framework

A modular, robust C/C++ testing and diagnostic framework designed to validate custom Linux character and miscellaneous device drivers through rigorous user-space and kernel-space interaction.

---

## Table of Contents

- [Project Overview](#project-overview)
- [Problem Statement](#problem-statement)
- [Objective](#objective)
- [Scope](#scope)
- [Key Features](#key-features)
- [Architecture](#architecture)
- [Technologies](#technologies)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Build Instructions](#build-instructions)
- [Driver Loading and Setup](#driver-loading-and-setup)
- [Running the Framework](#running-the-framework)
- [Test Cases](#test-cases)
- [Verified Test Results](#verified-test-results)
- [Reports and Logs](#reports-and-logs)
- [Documentation](#documentation)
- [Limitations](#limitations)
- [Future Improvements](#future-improvements)
- [Conclusion](#conclusion)

---

## Project Overview

The **Linux Device Driver Testing and Diagnostic Framework** is an academic capstone project developed in **C** and **C++17** to demonstrate automated verification, diagnostics, and reliability testing for a custom Linux kernel device driver.

The framework pairs a Linux kernel module (`vtest_driver.ko`) providing a virtual miscellaneous character device (`/dev/vtest0`) with an object-oriented C++ user-space testing engine (`driver-test`). The testing suite rigorously validates core driver file operations, custom IOCTL handling, boundary/buffer limits, error scenarios, and stress conditions under kernel-level mutex synchronization.

---

## Problem Statement

Developing and maintaining Linux kernel device drivers is critical for system reliability. Because device drivers execute in kernel space (Ring 0), defects such as unhandled boundary conditions, race conditions, memory leaks, or improper IOCTL validation can lead to kernel crashes, data corruption, or system lockups.

Traditional manual testing via ad-hoc terminal commands (`echo`, `cat`) lacks reproducibility, structured assertion verification, systematic error-path evaluation, and comprehensive test reporting. There is a need for a modular, automated testing framework that executes structured functional, error, boundary, and stress tests against kernel drivers while capturing reproducible diagnostic evidence.

---

## Objective

- Design and implement a Linux virtual character/misc device driver in C with kernel mutex protection.
- Build an object-oriented C++17 user-space test harness using a clean abstraction layer (`DriverInterface`).
- Verify fundamental POSIX character device operations: `open()`, `write()`, `read()`, `ioctl()`, and `close()`.
- Validate driver resilience against invalid IOCTL requests and payload overflows.
- Perform high-iteration stress testing (100 sequential cycles) to check for successful read/write behavior across repeated operations.
- Generate structured test reports and execution logs for diagnostics and capstone evaluation.

---

## Scope

- **In Scope:**
  - Linux kernel module development using the `miscdevice` subsystem.
  - Mutex-protected kernel device buffer management (256-byte static buffer, 255-byte usable payload).
  - User-space C++ wrapper encapsulating POSIX system calls.
  - Four distinct test classes: Functional, Error, Boundary, and Stress.
  - Pre-flight device diagnostic checking.
  - Formatted file-based logging (`reports/driver_test.log`) and summary reporting (`reports/test_report.txt`).
- **Out of Scope:**
  - Physical bus hardware (PCIe, USB, I2C, SPI) – testing is performed on a virtual kernel device node.
  - Third-party test runners or heavy runtime languages (Python, Java, Docker, or Google Test).
  - Dynamic runtime command-line argument configuration (the current suite runs all registered tests sequentially).

---

## Key Features

- **Kernel Mutex Synchronization**: Kernel shared state (`device_buffer` and `data_size`) is guarded with `mutex_lock_interruptible()` to ensure thread safety across concurrent operations.
- **Robust Boundary Enforcement**: The driver safely truncates oversized write payloads to its usable buffer limit (`BUFFER_SIZE - 1` = 255 bytes) without memory corruption or buffer overflows.
- **POSIX Error Validation**: The driver validates incoming IOCTL requests and explicitly returns `-EINVAL` on unknown commands, verified in user space via `errno`.
- **Pre-Flight Diagnostics**: `DiagnosticManager` checks `/dev/vtest0` accessibility before executing test suites to prevent confusing runtime crashes.
- **Object-Oriented Test Architecture**: Test cases inherit from an abstract `TestCase` interface and are executed uniformly through a centralized `TestController`.
- **Dual Reporting**: Produces both human-readable status summaries (`test_report.txt`) and event log streams (`driver_test.log`).

---

## Architecture

### Execution Stack

```text
+-----------------------------------------------------------+
|                    User Space (C++17)                     |
|                                                           |
|  [ main.cpp ]                                             |
|        │                                                  |
|        ├──> [ DiagnosticManager ] (Checks /dev/vtest0)    |
|        │                                                  |
|        └──> [ TestController ]                            |
|                  │                                        |
|                  ├──> [ TestCase Implementations ]        |
|                  │      • FunctionalTest                  |
|                  │      • ErrorTest                       |
|                  │      • BoundaryTest                    |
|                  │      • StressTest                      |
|                  │                                        |
|                  ├──> [ Logger ]  ──> driver_test.log     |
|                  └──> [ ReportGenerator ] ──> report.txt  |
|                             │                             |
|                             ▼                             |
|                  [ DriverInterface ]                      |
|             (open, write, read, ioctl, close)             |
+-----------------------------------------------------------+
                              │  POSIX System Calls
                              ▼
                      [ /dev/vtest0 ]
+-----------------------------------------------------------+
|                   Kernel Space (Ring 0)                   |
|                                                           |
|  [ vtest_driver.ko ] (Linux Misc Device Driver)           |
|        ├── vtest_open() / vtest_release()                 |
|        ├── vtest_read()                                   |
|        ├── vtest_write()                                  |
|        └── vtest_ioctl() (VTEST_IOCTL_GET_SIZE)           |
|                  │                                        |
|                  ▼ (device_mutex synchronization)         |
|        [ device_buffer (256 B) & data_size ]              |
+-----------------------------------------------------------+
```

### Component Interaction Flow

1. **Application Start**: `main()` instantiates `DiagnosticManager` to confirm `/dev/vtest0` is present.
2. **Test Orchestration**: `TestController` iterates through registered `TestCase` instances, dispatching execution, printing console feedback, and forwarding status messages to `Logger`.
3. **Driver Communication**: Test cases invoke `DriverInterface` methods, which issue POSIX system calls (`open`, `write`, `read`, `ioctl`, `close`) to the `/dev/vtest0` node.
4. **Kernel Processing**: The Linux VFS layer routes system calls to `vtest_fops`. The driver acquires `device_mutex`, performs safe user-kernel memory transfers (`copy_from_user` / `copy_to_user`), updates kernel logs (`pr_info`), and releases the mutex.
5. **Report Generation**: `ReportGenerator` compiles individual test outcomes into an itemized breakdown and calculates overall pass rate in `reports/test_report.txt`.

---

## Technologies

- **C**: Linux kernel module implementation (`vtest_driver.c`).
- **C++17**: Object-oriented user-space test engine and diagnostic manager.
- **Linux Kernel Subsystems**: `miscdevice` registration, `file_operations`, kernel mutexes (`<linux/mutex.h>`), user-space memory access (`<linux/uaccess.h>`), IOCTL infrastructure (`<linux/ioctl.h>`).
- **Build Tools**:
  - **CMake (>= 3.16)**: Configures and compiles the C++ user-space executable.
  - **GNU Make**: Invokes the Linux Kbuild module build system.
  - **GCC / G++**: Compilers for kernel C and modern C++17.
- **Git & GitHub**: Source code version control.

---

## Project Structure

```text
linux-device-driver-testing/
├── CMakeLists.txt              # CMake build configuration for C++ test harness
├── .gitignore                  # Git ignore rules for build artifacts and logs
├── README.md                   # Project documentation and user guide
├── docs/
│   └── testing.md              # Detailed Stage 5 test evidence and verification
├── driver/
│   ├── Makefile                # Kbuild Makefile for kernel module
│   ├── vtest_driver.c          # Linux misc character driver implementation
│   ├── vtest_driver.ko         # Compiled Linux kernel module
│   └── vtest_ioctl.h           # Driver-side IOCTL command definition
├── include/
│   ├── boundary_test.h         # Header: Boundary/buffer limit test
│   ├── diagnostic_manager.h    # Header: Device availability diagnostics
│   ├── driver_interface.h      # Header: User-space POSIX driver abstraction
│   ├── error_test.h            # Header: Invalid IOCTL error test
│   ├── functional_test.h       # Header: Core functional verification test
│   ├── logger.h                # Header: Event logging subsystem
│   ├── report_generator.h      # Header: Test report generator
│   ├── stress_test.h           # Header: 100-cycle stress test
│   ├── test_case.h             # Header: Abstract TestCase base class
│   ├── test_controller.h       # Header: Test execution orchestrator
│   └── vtest_ioctl.h           # Header: User-space IOCTL command definition
├── src/
│   ├── diagnostics/
│   │   └── diagnostic_manager.cpp # Diagnostics implementation
│   ├── driver/
│   │   └── driver_interface.cpp   # DriverInterface POSIX wrapper implementation
│   ├── logging/
│   │   └── logger.cpp             # Logger implementation
│   ├── main.cpp                   # Main application entry point
│   ├── reporting/
│   │   └── report_generator.cpp   # ReportGenerator implementation
│   └── testing/
│       ├── boundary_test.cpp      # BoundaryTest implementation
│       ├── error_test.cpp         # ErrorTest implementation
│       ├── functional_test.cpp    # FunctionalTest implementation
│       ├── stress_test.cpp        # StressTest implementation
│       ├── test_case.cpp          # TestCase implementation
│       └── test_controller.cpp    # TestController implementation
└── reports/
    ├── driver_test.log         # Execution log output
    └── test_report.txt         # Final formatted test summary report
```

---

## Requirements

The framework is developed and verified on an Ubuntu Linux environment. The following packages are required:

- **Operating System**: Ubuntu Linux or another compatible Linux distribution with matching kernel headers.
- **Linux Kernel Headers**: Matching the running kernel version (`linux-headers-$(uname -r)`)
- **Compilers**: GCC (C11 support) and G++ (C++17 support)
- **Build Utilities**: CMake (version 3.16 or higher) and GNU Make
- **Privileges**: `sudo` / root access required for kernel module loading and accessing `/dev/vtest0`

To verify kernel headers on your system:
```bash
ls -d /lib/modules/$(uname -r)/build
```

---

## Build Instructions

The project uses two separate build systems: GNU Make via Kbuild for the kernel driver, and CMake for the C++ application.

### 1. Build the Linux Kernel Module

```bash
make -C driver
```

This compiles `driver/vtest_driver.c` into the kernel module `driver/vtest_driver.ko`.

To clean driver build artifacts:
```bash
make -C driver clean
```

### 2. Build the C++ Testing Application

```bash
cmake -B build -S .
cmake --build build
```

This creates the executable `build/driver-test` using C++17.

---

## Driver Loading and Setup

Loading kernel modules and managing `/dev/` nodes requires administrative privileges:

### 1. Load the Kernel Module

```bash
sudo insmod driver/vtest_driver.ko
```

### 2. Verify Driver Registration

Check that the miscellaneous device node `/dev/vtest0` was automatically created:
```bash
ls -l /dev/vtest0
```
Expected output:
```text
crw------- 1 root root 10, <minor> ... /dev/vtest0
```

Verify module presence in the running kernel:
```bash
lsmod | grep vtest_driver
```

### 3. Unload the Driver (When Finished)

```bash
sudo rmmod vtest_driver
```

---

## Running the Framework

Run the test suite with root privileges (required to read/write the `/dev/vtest0` device node):

```bash
sudo ./build/driver-test
```

### Expected Output Sequence

```text
=== Linux Driver Diagnostic Framework ===
Diagnostic: /dev/vtest0 is available
Running: Functional Driver Test
[INFO] Starting test: Functional Driver Test
Read verification: PASS
IOCTL size verification: PASS
RESULT: PASS
[INFO] Test passed: Functional Driver Test
Running: Error Handling Test
[INFO] Starting test: Error Handling Test
Invalid ioctl test: PASS
RESULT: PASS
[INFO] Test passed: Error Handling Test
Running: Boundary and Buffer Limit Test
[INFO] Starting test: Boundary and Buffer Limit Test
Buffer limit verification: PASS
IOCTL boundary size verification: PASS
RESULT: PASS
[INFO] Test passed: Boundary and Buffer Limit Test
Running: Stress Test
[INFO] Starting test: Stress Test
Stress test iterations: 100
Stress test: PASS
RESULT: PASS
[INFO] Test passed: Stress Test
Test report generated: reports/test_report.txt
```

---

## Test Cases

### 1. Functional Driver Test (`FunctionalTest`)
- **File**: `src/testing/functional_test.cpp`
- **Objective**: Verify standard character device file operations end-to-end.
- **Workflow**:
  1. Opens `/dev/vtest0` via `DriverInterface::openDevice()`.
  2. Writes `"Functional Test"` (15 bytes) to the driver.
  3. Closes the device descriptor.
  4. Reopens the device.
  5. Reads data back and verifies byte-for-byte equality with the written string.
  6. Executes `VTEST_IOCTL_GET_SIZE` and verifies reported data size equals 15.
  7. Closes device.

### 2. Error Handling Test (`ErrorTest`)
- **File**: `src/testing/error_test.cpp`
- **Objective**: Validate the driver's rejection of unsupported commands.
- **Workflow**:
  1. Opens `/dev/vtest0`.
  2. Obtains the valid file descriptor via `DriverInterface::getFd()`.
  3. Dispatches an intentionally invalid IOCTL request (`0xDEADBEEF`).
  4. Confirms that `ioctl()` returns `-1` and `errno` is set to `EINVAL` (Invalid argument).
  5. Closes device.

### 3. Boundary and Buffer Limit Test (`BoundaryTest`)
- **File**: `src/testing/boundary_test.cpp`
- **Objective**: Verify that payloads larger than the driver's internal capacity are bounded safely.
- **Workflow**:
  1. Opens `/dev/vtest0`.
  2. Generates an oversized string of 300 bytes (`std::string(300, 'B')`).
  3. Writes payload to `/dev/vtest0`.
  4. Confirms the kernel driver bounds the write to its usable capacity (`BUFFER_SIZE - 1` = 255 bytes).
  5. Reopens device and queries `VTEST_IOCTL_GET_SIZE` to assert size equals 255.
  6. Reads back stored bytes to confirm zero corruption or memory overflow.
  7. Closes device.

### 4. Stress and Reliability Test (`StressTest`)
- **File**: `src/testing/stress_test.cpp`
- **Objective**: Ensure stability, synchronization, and resource cleanup across sustained cycles.
- **Workflow**:
  1. Executes 100 sequential iterations.
  2. Each iteration executes: `open` -> `write("Stress Test")` -> `close` -> `open` -> `read & verify` -> `close`.
  3. The test completes 100 sequential cycles and checks for successful read/write behavior across repeated operations.

---

## Verified Test Results

The latest execution of the framework produced the following verified output:

| Test Case Name | Category | Iterations | Status |
| :--- | :--- | :---: | :---: |
| **Functional Driver Test** | Basic I/O & IOCTL | 1 | **PASS** |
| **Error Handling Test** | Invalid IOCTL Validation | 1 | **PASS** |
| **Boundary and Buffer Limit Test** | Buffer Overflow Protection | 1 | **PASS** |
| **Stress Test** | Reliability & Resource Leaks | 100 | **PASS** |

**Summary Statistics:**
- Total Tests: **4**
- Passed: **4**
- Failed: **0**
- Pass Rate: **100.0%**
- Overall Framework Status: **ALL TESTS PASSED**

---

## Reports and Logs

The framework maintains persistent records of test executions in the `reports/` directory:

### 1. Test Report (`reports/test_report.txt`)
Contains the formatted summary generated by `ReportGenerator`:

```text
====================================
 Linux Driver Diagnostic Report
====================================

Test Details:
 - Functional Driver Test             : PASS
 - Error Handling Test                : PASS
 - Boundary and Buffer Limit Test     : PASS
 - Stress Test                        : PASS

Total Tests : 4
Passed      : 4
Failed      : 0
Pass Rate   : 100.0%

Status: ALL TESTS PASSED
```

### 2. Execution Log (`reports/driver_test.log`)
Contains categorized log entries (`[INFO]`, `[ERROR]`) recorded during test execution by `Logger`.

### 3. Kernel Ring Buffer (`dmesg`)
Kernel log inspection confirms proper driver lifecycle events:
```bash
sudo dmesg | tail -30
```
Verified events include device open/close tracking, read/write byte counts, and IOCTL size queries.

---

## Documentation

Comprehensive testing methodologies, design decisions, integration flows, and Stage 5 validation evidence are maintained in:

- **[`docs/testing.md`](docs/testing.md)**: Stage 5 Testing and Integration documentation detailing functional, error, boundary, stress, diagnostic, and kernel log observations.

---

## Limitations

- **Virtual Driver Target**: The framework targets a virtual character/misc device driver in RAM rather than physical bus controllers (PCIe, USB, Ethernet).
- **Sequential Execution**: All four test suites are executed sequentially in a single run; selective single-test filtering via CLI flags is not currently implemented.
- **Fixed Buffer Demonstration**: The driver buffer is statically sized at 256 bytes (`BUFFER_SIZE`) to demonstrate boundary and truncation behavior rather than dynamic kernel streaming.

---

## Future Improvements

The following enhancements represent potential future directions:

- **Command-Line Argument Parsing**: Adding a CLI argument parser (e.g., `--test <name>`, `--iterations <N>`, `--verbose`).
- **Additional IOCTL Commands**: Implementing driver reset, buffer clear, or configuration commands.
- **Concurrent Multi-Threaded Stress Tests**: Launching multiple user-space threads accessing `/dev/vtest0` simultaneously to stress-test kernel mutex contention.
- **Automated CI Integration**: Integrating virtual kernel runners (QEMU / UML) to execute the test suite in continuous integration environments.
- **Unified UAPI Header**: Consolidating `driver/vtest_ioctl.h` and `include/vtest_ioctl.h` into a shared UAPI header include path.

---

## Conclusion

The **Linux Device Driver Testing and Diagnostic Framework** successfully demonstrates a complete end-to-end testing pipeline for Linux kernel character device drivers. By combining kernel-level synchronization, robust user-space abstractions, systematic boundary and error validation, and automated reporting, the project provides a clean, reliable model for systems programming and device driver quality assurance.
