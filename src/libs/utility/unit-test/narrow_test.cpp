#include "test/test.hpp"
#include "utility/narrow.hpp"
#include <cstdint>

begin_suite(narrow_test);

test_case(simple)
{
    int16_t t{42};
    uint8_t u{42};

    assert_eq(gorc::narrow<uint8_t>(t), u);
}

test_case(overflow)
{
    int16_t t{256};
    assert_throws(gorc::narrow<uint8_t>(t), std::runtime_error, "Bad narrow cast");
}

test_case(underflow)
{
    int16_t t{-1};
    assert_throws(gorc::narrow<uint8_t>(t), std::runtime_error, "Bad narrow cast");
}

end_suite(narrow_test);
