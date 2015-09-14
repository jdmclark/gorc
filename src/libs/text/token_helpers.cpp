#include "token_helpers.hpp"
#include "utility/string_search.hpp"

void gorc::assert_identifier(tokenizer &t, std::string const &ident)
{
    if(t.get_type() == token_type::identifier &&
       iequal(t.get_value(), ident)) {
        t.advance();
    }
    else {
        diagnostic_context dc(t.get_location());
        LOG_FATAL(format("expected identifier '%s', found '%s'") %
                  ident %
                  t.get_value());
    }
}

void gorc::assert_punctuator(tokenizer &t, std::string const &punct)
{
    if(t.get_type() == token_type::punctuator &&
       iequal(t.get_value(), punct)) {
        t.advance();
    }
    else {
        diagnostic_context dc(t.get_location());
        LOG_FATAL(format("expected punctuator '%s', found '%s'") %
                  punct %
                  t.get_value());
    }
}
