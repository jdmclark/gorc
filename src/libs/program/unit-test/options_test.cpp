#include "test/test.hpp"
#include "program/options.hpp"
#include <queue>
#include <string>
#include <boost/format.hpp>

class options_test_fixture : public test::fixture {
public:
    gorc::options opts;
};

begin_suite_fixture(options_test, options_test_fixture);

test_case(load_empty_options_empty_argv)
{
    std::vector<std::string> arg_list;

    opts.load_from_arg_list(arg_list);

    assert_log_empty();
}

test_case(unknown_option_argv)
{
    std::vector<std::string> arg_list { "--bad-option" };

    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error,
                       "Unrecognized option --bad-option");
    assert_log_empty();
}

test_case(option_redefinition)
{
    bool a, b;
    opts.emplace<gorc::switch_option>("redefined-option", a);

    assert_throws_logged(opts.emplace<gorc::switch_option>("redefined-option", b));

    assert_log_message(gorc::log_level::error,
                       "Option redefined-option has already been defined");
    assert_log_empty();
}

test_case(name_contains_space)
{
    bool a;
    assert_throws_logged(opts.emplace<gorc::switch_option>("spaced option", a));

    assert_log_message(gorc::log_level::error,
                       "Option spaced option name cannot contain whitespace");
    assert_log_empty();
}

test_case(switch_type_set_unset)
{
    bool a;
    opts.emplace<gorc::switch_option>("user-option", a);

    assert_true(!opts.has_value("user-option"));

    std::vector<std::string> arg_list_1 { };

    opts.load_from_arg_list(arg_list_1);

    assert_true(!opts.has_value("user-option"));

    std::vector<std::string> arg_list_2 { "--user-option" };

    opts.load_from_arg_list(arg_list_2);

    assert_true(opts.has_value("user-option"));

    assert_log_empty();
}

test_case(switch_type_set_twice)
{
    bool a;
    opts.emplace<gorc::switch_option>("user-option", a);
    std::vector<std::string> arg_list { "--user-option", "--user-option" };
    opts.load_from_arg_list(arg_list);

    assert_log_message(gorc::log_level::warning,
                       "Option user-option is already set");
    assert_log_empty();
}

test_case(switch_type_get_switch)
{
    bool a;
    opts.emplace<gorc::switch_option>("user-option", a);

    auto const &sw = opts.get_option<gorc::switch_option>("user-option");
    assert_true(!sw.has_value());

    std::vector<std::string> arg_list { "--user-option" };

    opts.load_from_arg_list(arg_list);

    assert_true(sw.has_value());

    assert_log_empty();
}

test_case(switch_type_reset)
{
    bool a;
    opts.emplace<gorc::switch_option>("user-option", a);

    std::vector<std::string> arg_list_1 { "--user-option" };
    opts.load_from_arg_list(arg_list_1);
    assert_true(opts.has_value("user-option"));

    std::vector<std::string> arg_list_2;
    opts.load_from_arg_list(arg_list_2);
    assert_true(!opts.has_value("user-option"));
}

test_case(switch_type_mismatch)
{
    bool a;
    opts.emplace<gorc::switch_option>("user-option", a);

    assert_throws_logged((opts.get_option<gorc::value_option<int, int>>("user-option")));

    assert_log_message(gorc::log_level::error,
                       "Option user-option type mismatch");
    assert_log_empty();
}

test_case(switch_type_not_defined)
{
    assert_throws_logged(opts.get_option<gorc::switch_option>("user-option"));

    assert_log_message(gorc::log_level::error,
                       "Option user-option has not been defined");
    assert_log_empty();
}

test_case(value_type_set_unset)
{
    std::string a;
    opts.insert(gorc::make_value_option("user-option", a));

    assert_true(!opts.has_value("user-option"));

    std::vector<std::string> arg_list_1 { };

    opts.load_from_arg_list(arg_list_1);

    assert_true(!opts.has_value("user-option"));

    std::vector<std::string> arg_list_2 { "--user-option", "value" };

    opts.load_from_arg_list(arg_list_2);

    assert_true(opts.has_value("user-option"));

    assert_log_empty();
}

test_case(value_type_set_twice)
{
    std::string a;
    opts.insert(gorc::make_value_option("user-option", a));
    std::vector<std::string> arg_list { "--user-option", "value", "--user-option", "value2" };
    opts.load_from_arg_list(arg_list);

    auto const &sw = opts.get_option<gorc::value_option<std::string, std::string>>("user-option");
    assert_true(sw.has_value());
    assert_eq(a, "value2");

    assert_log_message(gorc::log_level::warning,
                       "Option user-option is already set");
    assert_log_empty();
}

test_case(value_type_get_switch)
{
    std::string a;
    opts.insert(gorc::make_value_option("user-option", a));

    auto const &sw = opts.get_option<gorc::value_option<std::string, std::string>>("user-option");
    assert_true(!sw.has_value());
    assert_true(a.empty());

    std::vector<std::string> arg_list { "--user-option", "value" };

    opts.load_from_arg_list(arg_list);

    assert_true(sw.has_value());
    assert_eq(a, "value");

    assert_log_empty();
}

test_case(value_type_reset)
{
    std::string a;
    opts.insert(gorc::make_value_option("user-option", a));

    std::vector<std::string> arg_list_1 { "--user-option", "value" };
    opts.load_from_arg_list(arg_list_1);
    assert_true(opts.has_value("user-option"));

    std::vector<std::string> arg_list_2;
    opts.load_from_arg_list(arg_list_2);
    assert_true(!opts.has_value("user-option"));

    auto const &sw = opts.get_option<gorc::value_option<std::string, std::string>>("user-option");
    assert_true(!sw.has_value());
    assert_true(a.empty());
}

test_case(value_type_mismatch)
{
    std::string a;
    opts.insert(gorc::make_value_option("user-option", a));

    assert_throws_logged(opts.get_option<gorc::switch_option>("user-option"));

    assert_log_message(gorc::log_level::error,
                       "Option user-option type mismatch");
    assert_log_empty();
}

test_case(multi_type_set_unset)
{
    std::vector<std::string> vals;
    opts.insert(gorc::make_multi_value_option("user-option", std::back_inserter(vals)));

    assert_true(!opts.has_value("user-option"));

    std::vector<std::string> arg_list_1 { };

    opts.load_from_arg_list(arg_list_1);

    assert_true(!opts.has_value("user-option"));

    std::vector<std::string> arg_list_2 { "--user-option", "value" };

    opts.load_from_arg_list(arg_list_2);

    assert_true(opts.has_value("user-option"));

    assert_log_empty();
}

test_case(multi_type_set_twice)
{
    std::vector<std::string> vals;
    opts.insert(gorc::make_multi_value_option("user-option", std::back_inserter(vals)));
    std::vector<std::string> arg_list { "--user-option", "value1", "--user-option", "value2" };
    opts.load_from_arg_list(arg_list);

    assert_eq(vals, (std::vector<std::string> { "value1", "value2" }));

    assert_log_empty();
}

test_case(multi_type_get_switch)
{
    std::vector<std::string> vals;
    opts.insert(gorc::make_multi_value_option("user-option", std::back_inserter(vals)));

    using mvo_t = gorc::multi_value_option<std::string,
                                            std::back_insert_iterator<std::vector<std::string>>>;

    auto const &sw = opts.get_option<mvo_t>("user-option");
    assert_true(!sw.has_value());

    std::vector<std::string> arg_list { "--user-option", "value1" };

    opts.load_from_arg_list(arg_list);

    assert_true(sw.has_value());
    assert_eq(vals, std::vector<std::string> { "value1" });

    assert_log_empty();
}

test_case(multi_type_reset)
{
    std::vector<int> vals;
    opts.insert(gorc::make_multi_value_option<int>("user-option", std::back_inserter(vals)));

    std::vector<std::string> arg_list_1 { "--user-option", "5" };
    opts.load_from_arg_list(arg_list_1);
    assert_true(opts.has_value("user-option"));

    std::vector<std::string> arg_list_2;
    opts.load_from_arg_list(arg_list_2);
    assert_true(!opts.has_value("user-option"));
}

test_case(multi_type_mismatch)
{
    bool b;
    opts.insert(gorc::make_switch_option("user-option", b));

    assert_throws_logged((opts.get_option<gorc::value_option<std::string, int>>("user-option")));

    assert_log_message(gorc::log_level::error,
                       "Option user-option type mismatch");
    assert_log_empty();
}

test_case(alias_not_defined)
{
    assert_throws_logged(opts.add_alias("user-option", "-uo"));

    assert_log_message(gorc::log_level::error,
                       "Option user-option has not been defined");
    assert_log_empty();
}

test_case(alias_redefinition)
{
    bool b;
    opts.insert(gorc::make_switch_option("user-option", b));
    opts.add_alias("user-option", "-uo");

    assert_throws_logged(opts.add_alias("user-option", "-uo"));

    assert_log_message(gorc::log_level::error,
                       "Option alias -uo has already been defined");
    assert_log_empty();
}

test_case(option_redefines_alias)
{
    bool a;
    opts.emplace<gorc::switch_option>("user-option", a);
    opts.add_alias("user-option", "--user-option-2");

    bool b;
    assert_throws_logged(opts.emplace<gorc::switch_option>("user-option-2", b));

    assert_log_message(gorc::log_level::error,
                       "Option alias --user-option-2 has already been defined");
    assert_log_empty();
}

test_case(value_option_not_enough_opts)
{
    std::string a;
    opts.insert(gorc::make_value_option("user-option", a));
    std::vector<std::string> arg_list { "--user-option" };

    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error,
                       "Option user-option requires a value");
    assert_log_empty();
}

test_case(multi_option_not_enough_opts)
{
    std::vector<int> vals;
    opts.insert(gorc::make_multi_value_option<int>("user-option", std::back_inserter(vals)));
    std::vector<std::string> arg_list { "--user-option" };

    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error,
                       "Option user-option requires a value");
    assert_log_empty();
}

test_case(bound_switch_test)
{
    bool value = true;
    bool value2 = false;
    opts.emplace<gorc::switch_option>("user-option", value);
    opts.emplace<gorc::switch_option>("user-option-2", value2);

    assert_true(value);
    assert_true(!value2);

    std::vector<std::string> arg_list_1;
    opts.load_from_arg_list(arg_list_1);

    assert_true(!value);
    assert_true(!value2);

    std::vector<std::string> arg_list_2 { "--user-option", "--user-option-2" };
    opts.load_from_arg_list(arg_list_2);

    assert_true(value);
    assert_true(value2);
}

test_case(bound_value_test)
{
    std::string strval;
    bool boolval = true;
    int intval = 5;

    opts.insert(gorc::make_value_option("strval", strval));
    opts.insert(gorc::make_value_option("boolval", boolval));
    opts.insert(gorc::make_value_option("intval", intval, 273));

    assert_true(strval.empty());
    assert_true(boolval);
    assert_eq(intval, 5);

    std::vector<std::string> arg_list_0;
    opts.load_from_arg_list(arg_list_0);

    assert_true(strval.empty());
    assert_true(!boolval);
    assert_eq(intval, 273);

    std::vector<std::string> arg_list_1 { "--boolval", "0",
                                          "--strval", "test_test_test",
                                          "--intval", "1874" };
    opts.load_from_arg_list(arg_list_1);

    assert_eq(strval, "test_test_test");
    assert_true(!boolval);
    assert_eq(intval, 1874);
}

test_case(invalid_bound_value_test)
{
    int intval = 5;

    opts.insert(gorc::make_value_option("intval", intval));
    std::vector<std::string> arg_list { "--intval", "not an integer" };

    assert_throws_logged(opts.load_from_arg_list(arg_list));

    assert_log_message(gorc::log_level::error,
                       "Could not parse value for option intval: "
                       "bad lexical cast: source type value could not be interpreted "
                       "as target");
    assert_log_empty();
}

test_case(bound_multi_value_test)
{
    std::vector<int> values = { 5, 4, 3 };

    opts.insert(gorc::make_multi_value_option<int>("intval", std::back_inserter(values)));

    using mvo_t = gorc::multi_value_option<int, std::back_insert_iterator<std::vector<int>>>;

    auto &mvo = opts.get_option<mvo_t>("intval");
    assert_true(!mvo.has_value());

    assert_eq(values, std::vector<int>({ 5, 4, 3 }));

    std::vector<std::string> arg_list_1;
    opts.load_from_arg_list(arg_list_1);
    assert_eq(values, std::vector<int>({ 5, 4, 3 }));

    assert_true(!mvo.has_value());

    std::vector<std::string> arg_list_2 { "--intval", "7" };
    opts.load_from_arg_list(arg_list_2);
    assert_eq(values, std::vector<int>({ 5, 4, 3, 7 }));

    assert_true(mvo.has_value());

    std::vector<std::string> arg_list_3 { "--intval", "10",
                                          "--intval", "8",
                                          "--intval", "6" };
    opts.load_from_arg_list(arg_list_3);
    assert_eq(values, std::vector<int>({ 5, 4, 3, 7, 10, 8, 6 }));

    assert_log_empty();

    std::vector<std::string> arg_list_4 { "--intval", "1038",
                                          "--intval", "not_a_number" };

    assert_throws_logged(opts.load_from_arg_list(arg_list_4));

    assert_log_message(gorc::log_level::error,
                       "Could not parse value for option intval: "
                       "bad lexical cast: source type value could not be interpreted "
                       "as target");
    assert_log_empty();

    assert_eq(values, std::vector<int>({5, 4, 3, 7, 10, 8, 6, 1038}));
}

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

end_suite(options_test);
