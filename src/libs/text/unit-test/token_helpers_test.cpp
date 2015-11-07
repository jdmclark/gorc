#include "test/test.hpp"
#include "text/token_helpers.hpp"
#include "io/memory_file.hpp"
#include "math/almost_equal.hpp"

using namespace gorc;

class token_helpers_fixture : public test::fixture {
public:
    memory_file mf;

    void set_file(std::string const &s)
    {
        mf.write(s.data(), s.size());
    }
};

begin_suite_fixture(token_helpers_test, token_helpers_fixture);

test_case(assert_identifier_not_identifier)
{
    set_file("%");
    generic_tokenizer t(mf);

    assert_throws_logged(assert_identifier(t, "id"));
    assert_log_message(log_level::error,
                       "<BUFFER>:1:1-1:1: expected identifier 'id', found '%'");
    assert_log_empty();
}

test_case(assert_identifier_non_matching)
{
    set_file("ia");
    generic_tokenizer t(mf);

    assert_throws_logged(assert_identifier(t, "id"));
    assert_log_message(log_level::error,
                       "<BUFFER>:1:1-1:2: expected identifier 'id', found 'ia'");
    assert_log_empty();
}

test_case(assert_identifier_success)
{
    set_file("id Xd yD KD");
    generic_tokenizer t(mf);

    assert_identifier(t, "id");
    assert_identifier(t, "xd");
    assert_identifier(t, "yd");
    assert_identifier(t, "kd");

    assert_log_empty();
}

test_case(assert_punctuator_not_punctuator)
{
    set_file("id");
    generic_tokenizer t(mf);

    assert_throws_logged(assert_punctuator(t, "%"));
    assert_log_message(log_level::error,
                       "<BUFFER>:1:1-1:2: expected punctuator '%', found 'id'");
    assert_log_empty();
}

test_case(assert_punctuator_not_matching)
{
    set_file(":");
    generic_tokenizer t(mf);

    assert_throws_logged(assert_punctuator(t, "%"));
    assert_log_message(log_level::error,
                       "<BUFFER>:1:1-1:1: expected punctuator '%', found ':'");
    assert_log_empty();
}

test_case(assert_punctuator_success)
{
    set_file(": % + -");
    generic_tokenizer t(mf);

    assert_punctuator(t, ":");
    assert_punctuator(t, "%");
    assert_punctuator(t, "+");
    assert_punctuator(t, "-");

    assert_log_empty();
}

test_case(get_number_format_failure)
{
    set_file("id");
    generic_tokenizer t(mf);

    assert_throws_logged(get_number<int>(t));
    assert_log_message(log_level::error,
                       "<BUFFER>:1:1-1:2: expected a numeric value, found 'id'");
}

test_case(get_number_format_int)
{
    set_file("5 0x10 010 122.3 3e2");
    generic_tokenizer t(mf);

    assert_eq(get_number<int>(t), 5);
    assert_eq(get_number<int>(t), 16);
    assert_eq(get_number<int>(t), 8);
    assert_eq(get_number<int>(t), 122);
    assert_eq(get_number<int>(t), 300);
}

test_case(get_number_format_real)
{
    set_file("5 0x10 010 122.3 3e2");
    generic_tokenizer t(mf);

    assert_true(almost_equal(get_number<float>(t), 5.0f));
    assert_true(almost_equal(get_number<float>(t), 16.0f));
    assert_true(almost_equal(get_number<float>(t), 8.0f));
    assert_true(almost_equal(get_number<float>(t), 122.3f));
    assert_true(almost_equal(get_number<float>(t), 300.0f));
}

end_suite(token_helpers_test);
