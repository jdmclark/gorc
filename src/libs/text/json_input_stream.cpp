#include "json_input_stream.hpp"
#include "log/log.hpp"
#include <sstream>

gorc::json_deserialization_constructor::json_deserialization_constructor()
{
    return;
}

gorc::json_input_stream::json_input_stream(input_stream &f)
    : tok(f)
{
    return;
}

gorc::json_input_stream::~json_input_stream()
{
    return;
}

gorc::diagnostic_context_location gorc::json_input_stream::get_diagnostic_context() const
{
    return tok.get_location();
}

void gorc::json_input_stream::advance_token()
{
    tok.advance();
    if(tok.get_type() == token_type::error) {
        diagnostic_context dc(get_diagnostic_context());
        LOG_FATAL(tok.get_reason());
    }
}

void gorc::json_input_stream::assert_token(std::string const &value)
{
    if(tok.get_value() != value) {
        diagnostic_context dc(get_diagnostic_context());
        LOG_FATAL(format("expected '%s', found '%s'") % value % tok.get_value());
    }

    advance_token();
}

int64_t gorc::json_input_stream::read_integer()
{
    bool negative = false;
    if(tok.get_value() == "-") {
        negative = true;
        advance_token();
    }
    else if(tok.get_value() == "+") {
        advance_token();
    }

    switch(tok.get_type()) {
    case token_type::integer:
    case token_type::hex_integer:
    case token_type::oct_integer:
        break;

    default: {
            diagnostic_context dc(get_diagnostic_context());
            LOG_FATAL("expected integer");
        }
    }

    auto v = std::stoll(tok.get_value(), nullptr, 0);

    return negative ? static_cast<int64_t>(-v) : static_cast<int64_t>(v);
}

double gorc::json_input_stream::read_double()
{
    bool negative = false;
    if(tok.get_value() == "-") {
        negative = true;
        advance_token();
    }
    else if(tok.get_value() == "+") {
        advance_token();
    }

    switch(tok.get_type()) {
    case token_type::integer:
    case token_type::real:
        break;

    default: {
            diagnostic_context dc(get_diagnostic_context());
            LOG_FATAL("expected real");
        }
    }

    auto v = std::stold(tok.get_value());
    return negative ? static_cast<double>(-v) : static_cast<double>(v);
}
