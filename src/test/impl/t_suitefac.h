#pragma once

#include "t_suite.h"
#include "t_reporter.h"

#include <memory>
#include <string>

#include <iostream>

namespace test {

class suite_factory {
public:
    const std::string filename;
    const int line_number;

    suite_factory(const std::string& f, int ln) : filename(f), line_number(ln) {
        return;
    }

    virtual ~suite_factory() {
        return;
    }

    virtual std::shared_ptr<suite> create_suite() = 0;
};

template <typename T> class specified_suite_factory : public suite_factory {
public:
    specified_suite_factory(const std::string& f, int ln) : suite_factory(f, ln) {
        return;
    }

    std::shared_ptr<suite> create_suite() {
        return std::make_shared<T>();
    }
};

}
