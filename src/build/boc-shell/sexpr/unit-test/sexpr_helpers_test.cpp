#include "test/test.hpp"
#include "../sexpr.hpp"
#include "../sexpr_helpers.hpp"
#include "utility/strcat.hpp"

using namespace gorc;

begin_suite(sexpr_helpers_test);

test_case(as_string_simple)
{
    assert_eq(as_string_value(make_sexpr("foo")), std::string("foo"));
}

test_case(as_string_cons)
{
    assert_throws_logged(as_string_value(make_sexpr(make_sexpr(), make_sexpr())));
    assert_log_message(log_level::error, "expected string, found data structure");
    assert_log_empty();
}

test_case(as_boolean_simple)
{
    assert_true(as_boolean_value(make_sexpr(true)));
    assert_true(!as_boolean_value(make_sexpr(false)));
}

test_case(as_boolean_not_boolean)
{
    assert_throws_logged(as_boolean_value(make_sexpr("maybe")));
    assert_log_message(log_level::error, "value 'maybe' is not boolean");
    assert_log_empty();
}

test_case(as_boolean_cons)
{
    assert_throws_logged(as_boolean_value(make_sexpr(make_sexpr(), make_sexpr())));
    assert_log_message(log_level::error, "expected boolean, found data structure");
    assert_log_empty();
}

test_case(nil_equal)
{
    assert_true(sexpr_equal(make_sexpr(), make_sexpr()));
}

test_case(atoms_equal)
{
    assert_true(sexpr_equal(make_sexpr("foo"), make_sexpr("foo")));
}

test_case(atoms_unequal)
{
    assert_true(!sexpr_equal(make_sexpr("foo"), make_sexpr("bar")));
}

test_case(atom_sexpr_unequal)
{
    assert_true(!sexpr_equal(make_sexpr(), make_sexpr(make_sexpr(), make_sexpr())));
}

test_case(cons_equal)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr("bar"));
    sexpr f = make_sexpr(make_sexpr("foo"), make_sexpr("bar"));

    assert_true(sexpr_equal(e, f));
}

test_case(cons_left_left_unequal)
{
    sexpr e = make_sexpr(make_sexpr("baz"), make_sexpr("bar"));
    sexpr f = make_sexpr(make_sexpr("foo"), make_sexpr("bar"));

    assert_true(!sexpr_equal(e, f));
}

test_case(cons_left_right_unequal)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr("baz"));
    sexpr f = make_sexpr(make_sexpr("foo"), make_sexpr("bar"));

    assert_true(!sexpr_equal(e, f));
}

test_case(cons_right_left_unequal)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr("bar"));
    sexpr f = make_sexpr(make_sexpr("baz"), make_sexpr("bar"));

    assert_true(!sexpr_equal(e, f));
}

test_case(cons_right_right_unequal)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr("bar"));
    sexpr f = make_sexpr(make_sexpr("foo"), make_sexpr("baz"));

    assert_true(!sexpr_equal(e, f));
}

test_case(empty_arg_to_argv)
{
    sexpr e = make_sexpr();
    auto rv = argument_to_argv(e);

    assert_true(rv.empty());
}

test_case(single_arg_to_argv)
{
    sexpr e = make_sexpr("foo");
    auto rv = argument_to_argv(e);
    std::vector<std::string> expected = { "foo" };

    assert_eq(rv, expected);
}

test_case(single_arg_to_argv_list)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr());
    auto rv = argument_to_argv(e);
    std::vector<std::string> expected = { "foo" };

    assert_eq(rv, expected);
}

test_case(multiple_arg_to_argv_list)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr(make_sexpr("bar"), make_sexpr()));
    auto rv = argument_to_argv(e);
    std::vector<std::string> expected = { "foobar" };

    assert_eq(rv, expected);
}

test_case(arg_containing_single_list)
{
    sexpr embedded = make_sexpr(make_sexpr("baz"), make_sexpr(make_sexpr("fnord"), make_sexpr()));
    sexpr containing =
        make_sexpr(
            make_sexpr("foo"),
            make_sexpr(
                embedded,
                make_sexpr(
                    make_sexpr("bar"),
                    make_sexpr())));
    auto rv = argument_to_argv(containing);
    std::vector<std::string> expected = { "foobaz", "fnordbar" };

    assert_eq(rv, expected);
}

test_case(arg_containing_multiple_lists)
{
    sexpr e1 = make_sexpr_from_range(std::vector<std::string>({ "foo", "bar", "baz" }));
    sexpr e2 = make_sexpr_from_range(std::vector<std::string>({ "fnord", "fizz", "buzz" }));
    sexpr containing =
        make_sexpr(
            make_sexpr("ack"),
            make_sexpr(
                e1,
                make_sexpr(
                    e2,
                    make_sexpr(
                        make_sexpr("farb"),
                        make_sexpr()))));
    auto rv = argument_to_argv(containing);
    std::vector<std::string> expected = { "ackfoo", "bar", "bazfnord", "fizz", "buzzfarb" };

    assert_eq(rv, expected);
}

test_case(arg_to_argv_depth_first_inorder)
{
    sexpr L1 = make_sexpr("L1");
    sexpr L2 = make_sexpr("L2");
    sexpr L3 = make_sexpr("L3");
    sexpr L4 = make_sexpr("L4");
    sexpr L5 = make_sexpr("L5");
    sexpr L6 = make_sexpr("L6");
    sexpr L7 = make_sexpr("L7");
    sexpr L8 = make_sexpr("L8");

    sexpr N1 = make_sexpr(L1, L2);
    sexpr N2 = make_sexpr(L3, L4);
    sexpr N3 = make_sexpr(L5, L6);
    sexpr N4 = make_sexpr(L7, L8);

    sexpr O1 = make_sexpr(N1, N2);
    sexpr O2 = make_sexpr(N3, N4);

    sexpr P = make_sexpr(O1, O2);

    sexpr Q = make_sexpr(P, make_sexpr());

    auto rv = argument_to_argv(Q);
    std::vector<std::string> expected = { "L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8" };

    assert_eq(rv, expected);
}

test_case(simple_arg_to_argv)
{
    sexpr e = make_sexpr(make_sexpr("foo"), make_sexpr(make_sexpr("bar"), make_sexpr()));
    auto rv = argument_to_argv(e);
    std::vector<std::string> expected = { "foobar" };

    assert_eq(rv, expected);
}

test_case(simple_arg_list_to_argv)
{
    sexpr e = make_sexpr_from_range(std::vector<std::string>({ "foo", "bar", "baz" }));
    auto rv = argument_list_to_argv(e);
    std::vector<std::string> expected = { "foo", "bar", "baz" };

    assert_eq(rv, expected);
}

test_case(nested_arg_list_to_argv)
{
    sexpr inner = make_sexpr_from_range(std::vector<std::string>({ "foo", "bar", "baz" }));
    sexpr arg =
        make_sexpr(
            make_sexpr("fnord"),
            make_sexpr(
                inner,
                make_sexpr(
                    make_sexpr("last"),
                    make_sexpr())));

    sexpr arglist =
        make_sexpr(
            make_sexpr("firstarg"),
            make_sexpr(
                arg,
                make_sexpr(
                    make_sexpr("lastarg"),
                    make_sexpr())));
    auto rv = argument_list_to_argv(arglist);
    std::vector<std::string> expected = { "firstarg", "fnordfoo", "bar", "bazlast", "lastarg" };

    assert_eq(rv, expected);
}

test_case(atom_to_argv)
{
    sexpr arg = make_sexpr("foo");
    auto rv = argument_list_to_argv(arg);
    std::vector<std::string> expected = { "foo" };
    assert_eq(rv, expected);
}

test_case(numeric_atom_to_numeric)
{
    assert_eq(as_numeric_value(make_sexpr("5")), 5);
    assert_eq(as_numeric_value(make_sexpr("-12")), -12);
    assert_eq(as_numeric_value(make_sexpr("0xFE")), 254);
}

test_case(non_numeric_atom_to_numeric)
{
    assert_throws_logged(as_numeric_value(make_sexpr("h3l10")));
    assert_log_message(log_level::error, "value 'h3l10' is not a number");
    assert_log_empty();
}

test_case(nil_to_numeric)
{
    assert_throws_logged(as_numeric_value(make_sexpr()));
    assert_log_message(log_level::error, "value '' is not a number");
    assert_log_empty();
}

test_case(big_number_to_numeric)
{
    assert_throws_logged(as_numeric_value(make_sexpr("0xFFFFFFFFFFFFFFFFF")));
    assert_log_message(log_level::error, "value '0xFFFFFFFFFFFFFFFFF' cannot be "
                                         "represented as an integer");
    assert_log_empty();
}

test_case(cons_to_numeric)
{
    assert_throws_logged(as_numeric_value(make_sexpr(make_sexpr(), make_sexpr())));
    assert_log_message(log_level::error, "expected number, found data structure");
    assert_log_empty();
}

end_suite(sexpr_helpers_test);
