#ifndef ERROR_TEST_H
#define ERROR_TEST_H

#include "test_case.h"

class ErrorTest : public TestCase {
public:
    std::string getName() const override;
    bool run() override;
};

#endif
