#pragma once

#include <string>
#include <sstream>
#include <functional>
#include "source.h"
#include "token.h"
#include "framework/diagnostics/report.h"
#include "framework/diagnostics/error_location.h"

namespace gorc {
namespace text {

class tokenizer {
private:
	diagnostics::report& ErrorReport;
	char current, next;
	source& buffer;
	diagnostics::error_location loc;
	int line, col;
	token internalToken;
	bool report_eol;

	inline void scan();

	inline bool skipWhitespaceStep();
	inline void skipWhitespace();

	inline bool isIdentifierLead(char c);
	inline bool isIdentifierChar(char c);

	void readNumericLiteralHexPart(token& out);
	void readNumericLiteralIntegerPart(token& out);
	void readNumericLiteral(token& out);
	void readIdentifier(token& out);
	void readStringLiteral(token& out);

public:
	tokenizer(source& stream, diagnostics::report& errorReport);

	void skip_to_next_line();

	inline void set_report_eol(bool val) {
		report_eol = val;
	}

	inline bool get_report_eol() const {
		return report_eol;
	}

	void get_token(token& out);
	void get_delimited_string(token& out, const std::function<bool(char)>& match_delim);

	template <typename T> T get_number() {
		get_token(internalToken);
		return internalToken.get_numeric_value<T>();
	}

	std::string& get_identifier();
	std::string& get_string_literal();
	std::string& get_space_delimited_string();

	void assert_identifier(const std::string& id);
	void assert_punctuator(const std::string& punc);
	void assert_label(const std::string& label);

	void assert_end_of_file();

	inline const text::location& get_internal_token_location() const {
		return internalToken.location;
	}
};

}
}
