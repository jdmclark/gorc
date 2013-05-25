#include "token.h"

Gorc::Text::Token::Token()
	: Type(TokenType::Invalid) {
	return;
}

Gorc::Text::Token::Token(TokenType type, const std::string& value, const Diagnostics::ErrorLocation& location)
	: Type(type), Value(value), Location(location) {
	return;
}
