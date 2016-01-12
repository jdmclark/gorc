#include "test/test.hpp"
#include "program/options.hpp"
#include <queue>
#include <string>
#include <boost/format.hpp>

namespace {
    class bare_options_test_fixture : public test::fixture {
    public:
        gorc::options opts;
    };
}

begin_suite_fixture(bare_options_test, bare_options_test_fixture);

test_case(bare_option_test)
{
    bool a;
    std::string b;

    std::vector<std::string> bare_opts;

    opts.emplace<gorc::switch_option>("foo", a);
    opts.insert(gorc::make_value_option("bar", b));

    opts.insert_bare(gorc::make_bare_multi_value_option(std::back_inserter(bare_opts)));

    std::vector<std::string> input_options { "arg1",
                                             "--foo",
                                             "arg2",
                                             "--bar",
                                             "arg3",
                                             "arg4" };

    opts.load_from_arg_list(input_options);
    assert_log_empty();

    assert_eq(bare_opts, (std::vector<std::string> { "arg1", "arg2", "arg4" }));
}

test_case(bare_multi_value_option_invalid_cast)
{
    std::vector<int> bare_opts;

    opts.insert_bare(gorc::make_bare_multi_value_option<int>(std::back_inserter(bare_opts)));

    std::vector<std::string> input_options { "5", "10", "potato", "12" };

    assert_throws_logged(opts.load_from_arg_list(input_options));
    assert_log_message(gorc::log_level::error,
                       "Could not parse value for input: bad lexical cast: "
                       "source type value could not be interpreted as target");
    assert_log_empty();
}

test_case(bare_option_has_value)
{
    std::vector<std::string> bare_opts;
    bool a;

    opts.insert_bare(gorc::make_bare_multi_value_option(std::back_inserter(bare_opts)));
    opts.insert(gorc::make_switch_option("foo", a));

    std::vector<std::string> arg_list_1 { "--foo" };

    opts.load_from_arg_list(arg_list_1);
    assert_true(!opts.has_bare_value());

    std::vector<std::string> arg_list_2 { "barevalue" };

    opts.load_from_arg_list(arg_list_2);
    assert_true(opts.has_bare_value());

    // Check reset
    opts.load_from_arg_list(arg_list_1);
    assert_true(!opts.has_bare_value());
}

test_case(at_least_one_input)
{
    std::vector<std::string> bare_opts;
    bool a;

    opts.insert_bare(gorc::make_bare_multi_value_option(std::back_inserter(bare_opts)));
    opts.insert(gorc::make_switch_option("foo", a));

    opts.emplace_constraint<gorc::at_least_one_input>();

    std::vector<std::string> arg_list_1 { "--foo" };
    assert_throws_logged(opts.load_from_arg_list(arg_list_1));
    assert_log_message(gorc::log_level::error,
                       "No input files specified");
    assert_log_empty();

    std::vector<std::string> arg_list_2 { "barevalue" };

    opts.load_from_arg_list(arg_list_2);
}

end_suite(bare_options_test);
