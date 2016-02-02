#include "test/test.hpp"
#include "content/fourcc.hpp"

using namespace gorc;

begin_suite(fourcc_test);

test_case(simple)
{
    fourcc epsilon("");
    assert_eq(static_cast<uint32_t>(epsilon), 0UL);

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

test_case(to_string)
{
    fourcc epsilon("");
    fourcc a("a");
    fourcc ab("ab");
    fourcc abc("abc");
    fourcc abcd("abcd");

    assert_eq(to_string(epsilon), std::string());
    assert_eq(to_string(a), std::string("a"));
    assert_eq(to_string(ab), std::string("ab"));
    assert_eq(to_string(abc), std::string("abc"));
    assert_eq(to_string(abcd), std::string("abcd"));
}

end_suite(fourcc_test);
