#include "functional_test.h"
#include "error_test.h"
#include "stress_test.h"
#include "test_controller.h"
#include "diagnostic_manager.h"
#include "report_generator.h"

#include <iostream>

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
    StressTest stressTest;

    bool functionalResult = controller.runTest(functionalTest);
    bool errorResult = controller.runTest(errorTest);
    bool stressResult = controller.runTest(stressTest);

    int passed = 0;
    int failed = 0;

    if (functionalResult)
        passed++;
    else
        failed++;

    if (errorResult)
        passed++;
    else
        failed++;

    if (stressResult)
        passed++;
    else
        failed++;

    ReportGenerator reportGenerator;
    reportGenerator.generateReport(passed, failed);

    return failed == 0 ? 0 : 1;
}
