# Software Requirements Specification (SRS)
## Linux Device Driver Testing and Diagnostic Framework

---

## 1. Requirements Overview

This document specifies the software requirements for the **Linux Device Driver Testing and Diagnostic Framework**. The framework provides an automated, structured environment for testing, validating, and diagnosing a custom Linux character/miscellaneous device driver from user space. 

The requirements defined herein reflect the actual system implementation, spanning the Linux kernel module (`vtest_driver.ko`), the object-oriented C++17 user-space test suite (`driver-test`), pre-flight diagnostic checks, and test report generation.

---

## 2. Project Objective

The primary objective is to design, implement, and validate a robust testing pipeline that verifies Linux device driver behavior across normal, boundary, error, and stress operating conditions. The framework bridges user-space testing abstractions with kernel-space execution, ensuring memory safety, proper synchronization, and accurate error reporting without requiring physical hardware peripherals.

---

## 3. Functional Requirements

### FR-1: Kernel Module Loading and Verification
- **FR-1.1**: The framework shall support compilation of the driver kernel module (`vtest_driver.ko`) via the Linux Kbuild system.
- **FR-1.2**: The driver shall register as a miscellaneous character device using `misc_register()` under the identifier `/dev/vtest0` with dynamic minor number assignment (`MISC_DYNAMIC_MINOR`).
- **FR-1.3**: The driver shall unregister cleanly upon module removal via `misc_deregister()`.

### FR-2: Device Detection and Pre-Flight Diagnostics
- **FR-2.1**: The framework shall provide a `DiagnosticManager` component that checks for the existence and accessibility of `/dev/vtest0` prior to running test suites.
- **FR-2.2**: If `/dev/vtest0` is unavailable or inaccessible, the framework shall report the diagnostic failure to `std::cerr` and terminate execution with a non-zero exit code (`1`) before running any test cases.

### FR-3: POSIX Device Operations Support
- **FR-3.1**: The driver and user-space abstraction layer (`DriverInterface`) shall support standard POSIX character device operations:
  - `open()`: Open the `/dev/vtest0` device and obtain a file descriptor.
  - `write()`: Transfer data from user space to kernel device memory.
  - `read()`: Transfer data from kernel device memory to user space.
  - `ioctl()`: Execute custom driver control commands.
  - `close()`: Release file descriptor context.

### FR-4: Functional Testing
- **FR-4.1**: The framework shall implement a `FunctionalTest` class that performs an end-to-end operational sequence:
  1. Open `/dev/vtest0`.
  2. Write a verified payload (`"Functional Test"`).
  3. Close and reopen the device.
  4. Read stored data back and assert byte-for-byte equality.
  5. Query stored data size using `VTEST_IOCTL_GET_SIZE` and verify it matches payload length (15 bytes).
  6. Close the device.

### FR-5: Error Handling Testing
- **FR-5.1**: The framework shall implement an `ErrorTest` class that evaluates the driver's rejection of invalid commands.
- **FR-5.2**: The test shall open `/dev/vtest0`, obtain the valid file descriptor via `DriverInterface::getFd()`, and issue an unsupported IOCTL command (`0xDEADBEEF`).
- **FR-5.3**: The driver shall return `-EINVAL` for unrecognized commands, causing the user-space `ioctl()` call to return `-1` with `errno` set to `EINVAL`.
- **FR-5.4**: The test shall verify this failure and report `PASS` when correctly rejected.

### FR-6: Boundary and Buffer-Limit Testing
- **FR-6.1**: The driver shall maintain an internal static buffer of 256 bytes (`BUFFER_SIZE`), providing a maximum usable payload capacity of 255 bytes (`BUFFER_SIZE - 1`) to reserve null termination.
- **FR-6.2**: The framework shall implement a `BoundaryTest` class that sends an oversized payload (300 bytes) to `/dev/vtest0`.
- **FR-6.3**: The driver shall safely truncate incoming writes exceeding 255 bytes via `min(count, BUFFER_SIZE - 1)`, preventing buffer overflow or memory corruption.
- **FR-6.4**: The test shall verify via `VTEST_IOCTL_GET_SIZE` that reported data size equals exactly 255 bytes and confirm the read-back payload matches the first 255 bytes.

### FR-7: Stress Testing
- **FR-7.1**: The framework shall implement a `StressTest` class that executes 100 sequential test iterations.
- **FR-7.2**: Each iteration shall execute an `open()`, `write()`, `close()`, `reopen()`, `read()`, content verification, and `close()` sequence.
- **FR-7.3**: The test shall verify consistent read/write behavior across repeated operations.

### FR-8: Shared State Mutex Synchronization
- **FR-8.1**: The driver shall protect its shared state (`device_buffer` and `data_size`) using a kernel mutex (`device_mutex`) declared with `DEFINE_MUTEX()`.
- **FR-8.2**: Read, write, and IOCTL operations shall acquire `device_mutex` via `mutex_lock_interruptible()` and release it on all return paths.

### FR-9: Test Report Generation
- **FR-9.1**: The framework shall provide a `ReportGenerator` component that records test outcomes into `reports/test_report.txt`.
- **FR-9.2**: The report shall contain an itemized test detail section listing each test's name and status (`PASS`/`FAIL`).
- **FR-9.3**: The report shall display aggregate metrics: Total Tests, Passed, Failed, Pass Rate percentage, and overall status string (`ALL TESTS PASSED` or `SOME TESTS FAILED`).

### FR-10: Execution Logging
- **FR-10.1**: The framework shall provide a `Logger` subsystem that logs execution events to standard console and appends them to `reports/driver_test.log`.
- **FR-10.2**: Log entries shall be categorized using level tags (`[INFO]`, `[ERROR]`).

---

## 4. Non-Functional Requirements

### NFR-1: Reliability
- The driver safely handles bounded input using the fixed buffer limit.
- Invalid IOCTL commands are rejected with `-EINVAL`.
- Mutex-protected shared state is used for read, write, and IOCTL operations.

### NFR-2: Maintainability
- The user-space framework shall follow object-oriented principles with a common abstract base class (`TestCase`) and an orchestrator (`TestController`).
- Hardware/driver interaction logic shall remain decoupled from test logic inside `DriverInterface`.

### NFR-3: Portability Across Compatible Linux Environments
- The kernel module shall build against standard Linux kernel module build systems (Kbuild) on compatible Linux environments with matching kernel headers.
- The user-space framework shall compile cleanly under C++17 across modern GCC/G++ toolchains.

### NFR-4: Performance
- User-space and kernel-space data transfers shall use direct kernel memory copying (`copy_to_user`, `copy_from_user`) with minimal operational overhead.
- Mutex lock contention shall be localized strictly to critical section memory accesses.

### NFR-5: Security and Access Control
- Access to `/dev/vtest0` follows the permissions assigned to the device node by the Linux environment. The test framework uses appropriate privileges when required to access the device.

### NFR-6: Extensibility
- New test cases can be integrated by subclassing `TestCase`, implementing `getName()` and `run()`, and registering the instance in `main.cpp`.

### NFR-7: Documentation
- The project shall maintain clear documentation covering operational guides (`README.md`), test verification evidence (`docs/testing.md`), and software requirements (`docs/requirements.md`).

---

## 5. Software Requirements

The framework requires the following software environment and toolchain:

| Component | Specification |
| :--- | :--- |
| **Operating System** | Ubuntu Linux or another compatible Linux distribution |
| **Kernel Headers** | Linux kernel headers matching the running kernel (`/lib/modules/$(uname -r)/build`) |
| **C Compiler** | GCC supporting C11 (for kernel driver compilation) |
| **C++ Compiler** | G++ supporting C++17 (for user-space framework) |
| **Build Systems** | CMake (version 3.16 or higher) and GNU Make |
| **Version Control** | Git |

---

## 6. Hardware Requirements

- **No physical hardware is required.**
- The framework operates entirely on a virtual Linux character/miscellaneous device driver instantiated in memory within the Linux kernel. All I/O operations execute against the virtual device node `/dev/vtest0`.

---

## 7. Project Constraints and Limitations

1. **Language Constraint**: Restricted strictly to C (kernel module) and C++17 (user-space application). No Python, Java, or third-party test libraries (e.g., GTest, Boost) are permitted or used.
2. **Platform Constraint**: Strictly Linux-specific due to direct reliance on the Linux kernel module subsystem, Kbuild, and POSIX character device system calls.
3. **Virtual Device Scope**: The driver is a virtual memory-backed character device intended for testing and diagnostics; it does not interface with physical hardware buses (PCIe, USB, I2C).
4. **Sequential Execution**: The test harness executes all registered test cases sequentially in a fixed single-run order.
5. **Fixed Driver Capacity**: The driver buffer is statically allocated at 256 bytes with a fixed usable payload capacity of 255 bytes.
6. **Command-Line Interface**: The current binary executes its full suite upon invocation; dynamic command-line flag/argument parsing is not yet implemented.

---

## 8. Deliverables

1. **Kernel Driver Package**:
   - `driver/vtest_driver.c`: Linux miscellaneous character device driver source code.
   - `driver/vtest_ioctl.h`: Kernel-side IOCTL command definition.
   - `driver/Makefile`: Kbuild Makefile for building `vtest_driver.ko`.
2. **User-Space Testing Framework**:
   - `include/`: Header files for `TestCase`, `TestController`, `DriverInterface`, `DiagnosticManager`, `Logger`, `ReportGenerator`, and test classes.
   - `src/`: C++ source implementations.
   - `CMakeLists.txt`: CMake build configuration.
   - `build/driver-test`: Compiled test runner binary.
3. **Reports and Evidence**:
   - `reports/test_report.txt`: Formatted test summary report.
   - `reports/driver_test.log`: Categorized execution event log.
4. **Documentation**:
   - `README.md`: Project overview, architecture, build and execution guide.
   - `docs/testing.md`: Stage 5 testing and integration verification evidence.
   - `docs/requirements.md`: Software requirements specification.

---

## 9. Acceptance Criteria

The framework is accepted when all the following verified criteria are satisfied:

| Acceptance Criterion | Verification Method | Status |
| :--- | :--- | :---: |
| **Device Detection** | `DiagnosticManager::checkDevice()` detects `/dev/vtest0` | **PASS** |
| **Functional Driver Test** | `FunctionalTest::run()` completes write, read, IOCTL size check | **PASS** |
| **Error Handling Test** | `ErrorTest::run()` verifies rejection of `0xDEADBEEF` with `EINVAL` | **PASS** |
| **Boundary & Buffer Limit Test** | `BoundaryTest::run()` verifies 300-byte write clamped to 255 bytes | **PASS** |
| **Stress Test** | `StressTest::run()` completes 100 sequential iterations | **PASS** |
| **Test Suite Pass Count** | 4 out of 4 tests pass successfully | **PASS** |
| **Test Suite Pass Rate** | Aggregate pass rate equals 100.0% (0 failed) | **PASS** |
| **Report Generation** | `reports/test_report.txt` is generated with itemized details | **PASS** |
