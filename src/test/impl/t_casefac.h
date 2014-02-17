#pragma once

namespace test {

class case_factory {
public:
    const std::string filename;
    const int line_number;

    case_factory(const std::string& f, int ln) : filename(f), line_number(ln) {
        return;
    }

    virtual ~case_factory() {
        return;
    }

    virtual case_object* create_case() = 0;
};

template <typename T> class specified_case_factory : public case_factory {
public:
    specified_case_factory(const std::string& f, int ln) : case_factory(f, ln) {
        return;
    }

    case_object* create_case() {
        return new T();
    }
};

}
