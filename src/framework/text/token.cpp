#include "token.h"

Gorc::Text::Token::Token()
	: Type(TokenType::Invalid) {
	return;
}

Gorc::Text::Token::Token(TokenType type, const std::string& value, const Text::Location& location)
	: Type(type), Value(value), Location(location) {
	return;
}
