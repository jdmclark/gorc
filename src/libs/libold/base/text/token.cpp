#include "token.hpp"

gorc::text::token::token()
    : type(token_type::invalid) {
    return;
}

gorc::text::token::token(token_type type, const std::string& value, const diagnostic_context_location& location)
    : type(type), value(value), location(location) {
    return;
}
