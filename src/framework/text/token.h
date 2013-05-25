#pragma once

#include <string>
#include <sstream>
#include "framework/diagnostics/errorlocation.h"
#include "framework/text/exception.h"

namespace Gorc {
namespace Text {

enum class TokenType {
	Invalid,
	EndOfFile,
	EndOfLine,
	Identifier,
	HexInteger,
	Integer,
	Float,
	String,
	Punctuator,
};

class Token {
public:
	TokenType Type;
	std::string Value;
	Diagnostics::ErrorLocation Location;

	Token();
	Token(TokenType Type, const std::string& Value, const Diagnostics::ErrorLocation& Location);

	template <typename T> T GetNumericValue() {
		bool formatSuccess = false;
		T result = T(0);

		std::stringstream ss(Value);

		if(Type == TokenType::Integer || Type == TokenType::Float) {
			formatSuccess = !(ss >> result).fail();
		}
		else if(Type == TokenType::HexInteger) {
			formatSuccess = !(ss >> std::hex >> result).fail();
		}

		if(!formatSuccess) {
			throw InvalidNumericConversionException();
		}

		return result;
	}
};

}
}
