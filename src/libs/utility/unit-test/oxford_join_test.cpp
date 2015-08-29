#include "test/test.hpp"
#include "utility/oxford_join.hpp"

#include <string>
#include <vector>

begin_suite(oxford_join_test);

test_case(empty_list_raises_error)
{
    std::vector<std::string> ems;

    assert_throws(gorc::oxford_join(ems),
                  std::runtime_error,
                  "cannot format empty list");
}

test_case(one_element_list)
{
    std::vector<std::string> ems { "ARG1" };

    assert_eq(gorc::oxford_join(ems),
              "ARG1");
}

test_case(two_element_list)
{
    std::vector<std::string> ems { "ARG1", "ARG2" };

    assert_eq(gorc::oxford_join(ems),
              "ARG1 and ARG2");
    assert_eq(gorc::oxford_join(ems, "or"),
              "ARG1 or ARG2");
}

test_case(three_element_list)
{
    std::vector<std::string> ems { "ARG1", "ARG2", "ARG3" };

    assert_eq(gorc::oxford_join(ems),
              "ARG1, ARG2, and ARG3");
    assert_eq(gorc::oxford_join(ems, "or"),
              "ARG1, ARG2, or ARG3");
}

end_suite(oxford_join_test);
