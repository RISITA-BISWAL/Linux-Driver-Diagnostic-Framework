cmake_minimum_required(VERSION 3.16)

project(LinuxDriverDiagnosticFramework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(driver-test
    src/main.cpp
    src/driver/driver_interface.cpp
    src/testing/test_case.cpp
    src/testing/functional_test.cpp
    src/testing/test_controller.cpp
    src/testing/error_test.cpp
    src/testing/stress_test.cpp
    src/diagnostics/diagnostic_manager.cpp
    src/logging/logger.cpp
    src/reporting/report_generator.cpp
)
