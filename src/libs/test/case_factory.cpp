#include "case_factory.hpp"

test::case_factory::case_factory(std::string const &f, int ln)
    : filename(f)
    , line_number(ln)
{
    return;
}

test::case_factory::~case_factory()
{
    return;
}
