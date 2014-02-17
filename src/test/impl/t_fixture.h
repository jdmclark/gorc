#pragma once

#include "t_case.h"

namespace test {

/// @brief The base type for all test fixtures.
class fixture : public case_object {
public:
    virtual ~fixture();

    virtual void setup();
    virtual void teardown();
};

}
