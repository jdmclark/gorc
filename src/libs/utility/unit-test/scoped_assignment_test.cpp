#include "test/test.hpp"
#include "utility/scoped_assignment.hpp"

begin_suite(scoped_assignment);

test_case(simple)
{
    int var = 5;
    {
        assert_eq(var, 5);
        auto var_guard = gorc::make_scoped_assignment(var, 10);
        assert_eq(var, 10);
    }

    assert_eq(var, 5);
}

test_case(move)
{
    int var_a = 5;
    int var_b = 10;

    {
        auto var_a_guard = gorc::make_scoped_assignment(var_a, 15);

        assert_eq(var_a, 15);
        assert_eq(var_b, 10);

        {
            auto var_b_guard = gorc::make_scoped_assignment(var_b, 20);

            assert_eq(var_a, 15);
            assert_eq(var_b, 20);

            var_a_guard = std::move(var_b_guard);

            assert_eq(var_a, 5);
            assert_eq(var_b, 20);
        }

        assert_eq(var_a, 5);
        assert_eq(var_b, 20);
    }

    assert_eq(var_a, 5);
    assert_eq(var_b, 10);
}

end_suite(scoped_assignment);
