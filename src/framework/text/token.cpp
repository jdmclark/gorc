#include "token.h"

gorc::text::token::token()
	: type(token_type::invalid) {
	return;
}

gorc::text::token::token(token_type type, const std::string& value, const text::location& location)
	: type(type), value(value), location(location) {
	return;
}
