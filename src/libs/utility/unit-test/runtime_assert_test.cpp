#include "test/test.hpp"
#include "utility/runtime_assert.hpp"

begin_suite(assert_test);

test_case(assert_passes)
{
    try {
        gorc::runtime_assert(true, "assert not thrown");
    }
    catch(...) {
        assert_always("exception thrown");
    }
}

test_case(assert_fails)
{
    assert_throws(gorc::runtime_assert(false, "assert thrown"),
                  std::runtime_error,
                  "assert thrown");
}

end_suite(assert_test);
