#include "test/test.hpp"
#include "program/options.hpp"
#include <queue>
#include <string>
#include <boost/format.hpp>

namespace {
    class option_constraint_test_fixture : public test::fixture {
    public:
        gorc::options opts;
    };
}

begin_suite_fixture(option_constraint_test, option_constraint_test_fixture);

test_case(constraint_failure_test)
{
    std::vector<std::string> arg_list { };

    class always_fail_constraint : public gorc::option_constraint {
    public:
        virtual void check_constraint(gorc::options const &) const override {
            LOG_FATAL("foo");
        }
    };

    opts.emplace_constraint<always_fail_constraint>();

    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error, "foo");
    assert_log_empty();
}

test_case(required_option_test)
{
    bool a;
    opts.emplace<gorc::switch_option>("foo", a);
    opts.emplace_constraint<gorc::required_option>("foo");

    std::vector<std::string> arg_list { };
    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error,
                       "Option foo must be specified");
    assert_log_empty();

    std::vector<std::string> arg_list2 { "--foo" };
    opts.load_from_arg_list(arg_list2);
}

test_case(required_option_list_test)
{
    bool a;
    opts.emplace<gorc::switch_option>("foo", a);
    bool b;
    opts.emplace<gorc::switch_option>("bar", b);
    bool c;
    opts.emplace<gorc::switch_option>("baz", c);
    opts.emplace_constraint<gorc::required_option>(std::vector<std::string> {"foo", "baz" });

    std::vector<std::string> arg_list { };
    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error,
                       "Options foo and baz must be specified");
    assert_log_empty();

    std::vector<std::string> arg_list2 { "--foo" };
    assert_throws_logged(opts.load_from_arg_list(arg_list2));

    assert_log_message(gorc::log_level::error,
                       "Options foo and baz must be specified");
    assert_log_empty();

    std::vector<std::string> arg_list3 { "--foo", "--baz" };
    opts.load_from_arg_list(arg_list3);
}

test_case(mutual_exclusion_test)
{
    bool a, b, c;

    opts.emplace<gorc::switch_option>("foo", a);
    opts.emplace<gorc::switch_option>("bar", b);
    opts.emplace<gorc::switch_option>("baz", c);

    std::vector<std::string> exclusive_options { "foo", "bar", "baz" };
    opts.emplace_constraint<gorc::mutual_exclusion>(exclusive_options,
                                                    /*min*/1,
                                                    /*max*/2);

    std::vector<std::string> arg_list_0 { };
    assert_throws_logged(opts.load_from_arg_list(arg_list_0));
    assert_log_message(gorc::log_level::error,
                       "At least 1 of foo, bar, and baz must be specified");
    assert_log_empty();

    std::vector<std::string> arg_list_1 { "--foo" };
    opts.load_from_arg_list(arg_list_1);
    assert_log_empty();

    std::vector<std::string> arg_list_2 { "--foo", "--bar" };
    opts.load_from_arg_list(arg_list_2);
    assert_log_empty();

    std::vector<std::string> arg_list_3 { "--foo", "--bar", "--baz" };
    assert_throws_logged(opts.load_from_arg_list(arg_list_3));
    assert_log_message(gorc::log_level::error,
                       "At most 2 of foo, bar, and baz may be specified");
    assert_log_empty();
}

test_case(dependent_option_test)
{
    bool a;
    bool b;

    opts.insert(gorc::make_switch_option("foo", a));
    opts.insert(gorc::make_switch_option("bar", b));

    opts.emplace_constraint<gorc::dependent_option>("foo", "bar");

    std::vector<std::string> arg_list_1 { "--bar" };
    opts.load_from_arg_list(arg_list_1);
    assert_log_empty();

    std::vector<std::string> arg_list_2 { "--foo", "--bar" };
    opts.load_from_arg_list(arg_list_2);
    assert_log_empty();

    std::vector<std::string> arg_list_3 { "--foo" };
    assert_throws_logged(opts.load_from_arg_list(arg_list_3));
    assert_log_message(gorc::log_level::error,
                       "option foo requires option bar");
}

end_suite(option_constraint_test);
