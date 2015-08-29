#include "suite_factory.hpp"

test::suite_factory::suite_factory(std::string const &f, int ln)
    : filename(f)
    , line_number(ln)
{
    return;
}

test::suite_factory::~suite_factory()
{
    return;
}
