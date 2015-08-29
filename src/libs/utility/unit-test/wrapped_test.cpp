#include "test/test.hpp"
#include "utility/wrapped.hpp"

begin_suite(wrapped_test);

test_case(simple)
{
    auto output = gorc::wrapped("simple test case");
    std::string expected = "simple test case";
    assert_eq(output, expected);
}

test_case(tagged)
{
    auto output = gorc::wrapped("INFO", "simple test case");
    std::string expected = "[INFO] simple test case";
    assert_eq(output, expected);
}

test_case(simple_wrapped)
{
    auto output = gorc::wrapped(
        "more_complex_test_case more_complex_test_case more_complex_test_case "
        "more_complex_test_case");
    std::string expected =
        "more_complex_test_case more_complex_test_case more_complex_test_case\n"
        "more_complex_test_case";
    assert_eq(output, expected);
}

test_case(tagged_wrapped)
{
    auto output = gorc::wrapped("INFO",
        "more_complex_test_case more_complex_test_case more_complex_test_case "
        "more_complex_test_case");
    std::string expected =
        "[INFO] more_complex_test_case more_complex_test_case more_complex_test_case\n"
        "       more_complex_test_case";
    assert_eq(output, expected);
}

test_case(longer_tagged_wrapped)
{
    auto output = gorc::wrapped("INFORMATION",
        "more_complex_test_case more_complex_test_case more_complex_test_case "
        "more_complex_test_case");
    std::string expected =
        "[INFORMATION] more_complex_test_case more_complex_test_case\n"
        "              more_complex_test_case more_complex_test_case";
    assert_eq(output, expected);
}

test_case(longer_than_line)
{
    auto output = gorc::wrapped("INFORMATION",
        "more_complex_test_case more_complex_test_case more_complex_test_case "
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    std::string expected =
        "[INFORMATION] more_complex_test_case more_complex_test_case\n"
        "              more_complex_test_case\n"
        "              aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    assert_eq(output, expected);
}

test_case(newline_in_message)
{
    auto output = gorc::wrapped("INFORMATION",
        "more_complex_test_case more_complex_\ntest_case more_complex_test_case "
        "more_complex_test_case");
    std::string expected =
        "[INFORMATION] more_complex_test_case more_complex_\n"
        "              test_case more_complex_test_case more_complex_test_case";
    assert_eq(output, expected);
}

test_case(narrower)
{
    auto output = gorc::wrapped("INFO", "a a a a a a a a a a", 10);
    std::string expected =
        "[INFO] a a\n"
        "       a a\n"
        "       a a\n"
        "       a a\n"
        "       a a";
    assert_eq(output, expected);
}

end_suite(wrapped_test);
