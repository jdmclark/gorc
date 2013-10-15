#pragma once

#include <string>
#include <sstream>
#include "framework/text/location.h"
#include "framework/text/exception.h"

namespace gorc {
namespace text {

enum class token_type {
	invalid,
	end_of_file,
	end_of_line,
	identifier,
	hex_integer,
	integer,
	floating,
	string,
	punctuator,
};

class token {
public:
	token_type type;
	std::string value;
	text::location location;

	token();
	token(token_type type, const std::string& value, const text::location& location);

	template <typename T> T get_numeric_value() {
		bool formatSuccess = false;
		T result = T(0);

		std::stringstream ss(value);

		if(type == token_type::integer || type == token_type::floating) {
			formatSuccess = !(ss >> result).fail();
		}
		else if(type == token_type::hex_integer) {
			formatSuccess = !(ss >> std::hex >> result).fail();
		}

		if(!formatSuccess) {
			throw invalid_numeric_conversion_exception();
		}

		return result;
	}
};

}
}
