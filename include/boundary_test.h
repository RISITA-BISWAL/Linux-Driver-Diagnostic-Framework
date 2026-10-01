#ifndef BOUNDARY_TEST_H
#define BOUNDARY_TEST_H

#include "test_case.h"

class BoundaryTest : public TestCase {
public:
    std::string getName() const override;
    bool run() override;
};

#endif
