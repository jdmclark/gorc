#include "test/test.hpp"
#include "program/options.hpp"
#include "program/subcommand_option.hpp"
#include "program/subcommand_registry.hpp"
#include "program/subcommand.hpp"
#include <queue>
#include <string>
#include <boost/format.hpp>

namespace {
    class subcommand_test_fixture : public test::fixture {
    public:
        gorc::options opts;
        gorc::subcommand_registry reg;
        gorc::maybe<int> value;

        subcommand_test_fixture()
        {
            opts.insert_bare(gorc::make_subcommand_option(reg, value));
        }
    };
}

begin_suite_fixture(subcommand_test, subcommand_test_fixture);

test_case(empty_registry_test)
{
    std::vector<std::string> empty_opts;
    opts.load_from_arg_list(empty_opts);

    assert_true(!value.has_value());
}

test_case(not_found_command)
{
    std::vector<std::string> args { "foobar" };

    assert_throws_logged(opts.load_from_arg_list(args));
    assert_log_message(gorc::log_level::error,
                       "Unknown subcommand foobar");
    assert_log_empty();

    assert_true(!value.has_value());
}

test_case(simple_bound_subcommand_value)
{
    class my_subcommand : public gorc::subcommand {
    public:
        int foo_value;
        int bar_value;

        virtual void create_options(gorc::options &opts) override
        {
            opts.insert(gorc::make_value_option("foo", foo_value, 5));
            opts.insert(gorc::make_value_option("bar", bar_value, 10));

            opts.emplace_constraint<gorc::required_option>("foo");
        }

        virtual int main() override
        {
            return foo_value + bar_value;
        }
    } my_sub, my_other;

    reg.add_subcommand("my_sub", my_sub);
    reg.add_subcommand("my_other", my_other);

    std::vector<std::string> missing_foo_opts { "my_sub", "--bar", "3" };
    assert_throws_logged(opts.load_from_arg_list(missing_foo_opts));
    assert_log_message(gorc::log_level::error, "Option foo must be specified");
    assert_log_empty();

    assert_true(!opts.has_bare_value());
    assert_true(!value.has_value());

    std::vector<std::string> good_opts { "my_other", "--foo", "1", "--bar", "4" };
    opts.load_from_arg_list(good_opts);
    assert_log_empty();

    assert_true(opts.has_bare_value());
    assert_true(value.has_value());
    assert_eq(value.get_value(), 5);
}

end_suite(subcommand_test);
