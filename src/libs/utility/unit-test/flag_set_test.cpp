#include "test/test.hpp"
#include "utility/flag_set.hpp"

begin_suite(flag_set_test);

test_case(initialize_to_zero)
{
    enum class test_enum {
        value
    };

    gorc::flag_set<test_enum> fs;

    assert_eq(static_cast<int>(fs), 0);
}

test_case(initialize_with_forced_value)
{
    enum class test_enum {
        valuex = 2,
        valuey = 4,
        valuez = 8
    };

    gorc::flag_set<test_enum> fs(10);

    assert_eq(static_cast<int>(fs), 10);

    assert_true(fs & test_enum::valuex);
    assert_true(!(fs & test_enum::valuey));
    assert_true(fs & test_enum::valuez);
}

test_case(initialize_with_init_list)
{
    enum class test_enum {
        valuex = 2,
        valuey = 4,
        valuez = 8
    };

    gorc::flag_set<test_enum> fs = { test_enum::valuex, test_enum::valuez };

    assert_eq(static_cast<int>(fs), 10);
}

test_case(test_intersection)
{
    enum class test_enum {
        valuex = 2,
        valuey = 4,
        valuez = 8
    };

    gorc::flag_set<test_enum> fs = { test_enum::valuex };
    gorc::flag_set<test_enum> ft = { test_enum::valuex, test_enum::valuez };
    gorc::flag_set<test_enum> fu = { test_enum::valuey };

    assert_true(fs & fs);
    assert_true(fs & ft);
    assert_true(ft & fs);
    assert_true(!(fs & fu));
    assert_true(!(ft & fu));
}

test_case(test_insert)
{
    enum class test_enum {
        valuex = 2,
        valuey = 4,
        valuez = 8
    };

    gorc::flag_set<test_enum> fs;

    assert_eq(static_cast<int>(fs), 0);
    assert_eq(static_cast<int>(fs + test_enum::valuey), 4);

    fs += test_enum::valuex;

    assert_eq(static_cast<int>(fs), 2);
}

test_case(test_union)
{
    enum class test_enum {
        valuex = 2,
        valuey = 4,
        valuez = 8
    };

    gorc::flag_set<test_enum> fs;
    gorc::flag_set<test_enum> ft = { test_enum::valuex };
    gorc::flag_set<test_enum> fu = { test_enum::valuex, test_enum::valuez };
    gorc::flag_set<test_enum> fv = { test_enum::valuez };

    assert_eq(ft + fv, fu);

    assert_ne(fs, ft);
    assert_ne(fs, fu);

    fs += ft;

    assert_eq(fs, ft);
    assert_ne(fs, fu);

    fs += fu;

    assert_eq(fs, fu);
}

test_case(test_erase)
{
    enum class test_enum {
        valuex = 2,
        valuey = 4,
        valuez = 8
    };

    gorc::flag_set<test_enum> fs;
    gorc::flag_set<test_enum> ft = { test_enum::valuex, test_enum::valuey };

    assert_eq(fs - ft, fs);
    assert_eq(ft - fs, ft);
    assert_eq(static_cast<int>(ft - test_enum::valuey), 2);

    ft -= test_enum::valuex;

    assert_eq(static_cast<int>(ft), 4);

    gorc::flag_set<test_enum> fu = { test_enum::valuex, test_enum::valuey };
    gorc::flag_set<test_enum> fv = { test_enum::valuex, test_enum::valuez };

    fu -= fv;

    assert_eq(fu, ft);
}

end_suite(flag_set_test);
