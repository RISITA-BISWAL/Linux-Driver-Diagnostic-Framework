#include "functional_test.h"
#include "error_test.h"
#include "boundary_test.h"
#include "stress_test.h"
#include "test_controller.h"
#include "diagnostic_manager.h"
#include "report_generator.h"

#include <iostream>
#include <vector>

int main()
{
    DiagnosticManager diagnostic;

    std::cout << "=== Linux Driver Diagnostic Framework ===\n";

    diagnostic.printStatus();

    if (!diagnostic.checkDevice())
    {
        std::cerr << "Device is unavailable. Tests cannot continue.\n";
        return 1;
    }

    TestController controller;

    FunctionalTest functionalTest;
    ErrorTest errorTest;
    BoundaryTest boundaryTest;
    StressTest stressTest;

    std::vector<TestResult> testResults;
    testResults.push_back({functionalTest.getName(), controller.runTest(functionalTest)});
    testResults.push_back({errorTest.getName(), controller.runTest(errorTest)});
    testResults.push_back({boundaryTest.getName(), controller.runTest(boundaryTest)});
    testResults.push_back({stressTest.getName(), controller.runTest(stressTest)});

    ReportGenerator reportGenerator;
    reportGenerator.generateReport(testResults);

    int failed = 0;
    for (const auto& res : testResults)
    {
        if (!res.passed)
            failed++;
    }

    return failed == 0 ? 0 : 1;
}
