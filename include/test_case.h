#ifndef TEST_CASE_H
#define TEST_CASE_H

#include <string>

class TestCase {
public:
    virtual ~TestCase() = default;

    virtual std::string getName() const = 0;
    virtual bool run() = 0;
};

#endif
