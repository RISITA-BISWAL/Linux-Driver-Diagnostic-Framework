#ifndef STRESS_TEST_H
#define STRESS_TEST_H

#include "test_case.h"

class StressTest : public TestCase {
public:
    std::string getName() const override;
    bool run() override;
};

#endif
