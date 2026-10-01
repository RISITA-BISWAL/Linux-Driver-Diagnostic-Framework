#ifndef FUNCTIONAL_TEST_H
#define FUNCTIONAL_TEST_H

#include "test_case.h"

class FunctionalTest : public TestCase {
public:
    std::string getName() const override;
    bool run() override;
};

#endif
