#include "test/test.hpp"
#include "content/fourcc.hpp"

using namespace gorc;

begin_suite(fourcc_test);

test_case(simple)
{
    fourcc a("a");
    assert_eq(static_cast<uint32_t>(a), 0x61000000UL);

    fourcc ab("ab");
    assert_eq(static_cast<uint32_t>(ab), 0x61620000UL);

    fourcc abc("abc");
    assert_eq(static_cast<uint32_t>(abc), 0x61626300UL);

    fourcc abcd("abcd");
    assert_eq(static_cast<uint32_t>(abcd), 0x61626364UL);
}

test_case(too_large)
{
    assert_throws(fourcc("abcde"),
                  std::runtime_error,
                  "fourcc size is too large");
}

test_case(equality)
{
    fourcc abcd("abcd");
    fourcc abcd2("abcd");

    assert_eq(abcd, abcd2);
    assert_true(abcd == abcd2);
    assert_true(!(abcd != abcd2));
}

test_case(inequality)
{
    fourcc abcd("abcd");
    fourcc abce("abce");

    assert_ne(abcd, abce);
    assert_true(abcd != abce);
    assert_true(!(abcd == abce));
}

end_suite(fourcc_test);
