#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <string>
#include <vector>

struct TestResult {
    std::string testName;
    bool passed;
};

class ReportGenerator {
public:
    void generateReport(const std::vector<TestResult>& results);
    void generateReport(int passed, int failed);
};

#endif
