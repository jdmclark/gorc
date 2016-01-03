#include "test/test.hpp"
#include "../self.hpp"

begin_suite(self_test);

test_case(simple)
{
    auto p = gorc::get_self_executable_path();

    assert_eq(p.stem().generic_string(), std::string("system-test"));
}

end_suite(self_test);
