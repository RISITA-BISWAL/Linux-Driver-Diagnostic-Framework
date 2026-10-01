#include "report_generator.h"

#include <fstream>
#include <iostream>

void ReportGenerator::generateReport(int passed, int failed)
{
    const int total = passed + failed;

    std::ofstream report("reports/test_report.txt");

    if (!report)
    {
        std::cerr << "Failed to create test report\n";
        return;
    }

    report << "====================================\n";
    report << " Linux Driver Diagnostic Report\n";
    report << "====================================\n\n";

    report << "Total Tests : " << total << '\n';
    report << "Passed      : " << passed << '\n';
    report << "Failed      : " << failed << '\n';

    if (total > 0)
    {
        double percentage =
            (static_cast<double>(passed) / total) * 100.0;

        report << "Pass Rate   : " << percentage << "%\n";
    }

    report << "\nStatus: "
           << (failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED")
           << '\n';

    report.close();

    std::cout << "Test report generated: reports/test_report.txt\n";
}
