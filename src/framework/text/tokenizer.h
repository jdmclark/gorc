#pragma once

#include <string>
#include <sstream>
#include <functional>
#include "source.h"
#include "token.h"
#include "framework/diagnostics/report.h"
#include "framework/diagnostics/error_location.h"

namespace Gorc {
namespace Text {

class Tokenizer {
private:
	Diagnostics::Report& ErrorReport;
	char current, next;
	Source& buffer;
	Diagnostics::ErrorLocation loc;
	int line, col;
	Token internalToken;
	bool report_eol;

	inline void scan();

	inline bool skipWhitespaceStep();
	inline void skipWhitespace();

	inline bool isIdentifierLead(char c);
	inline bool isIdentifierChar(char c);

	void readNumericLiteralHexPart(Token& out);
	void readNumericLiteralIntegerPart(Token& out);
	void readNumericLiteral(Token& out);
	void readIdentifier(Token& out);
	void readStringLiteral(Token& out);

public:
	Tokenizer(Source& stream, Diagnostics::Report& errorReport);

	void SkipToNextLine();

	inline void SetReportEOL(bool val) {
		report_eol = val;
	}

	inline bool GetReportEOL() const {
		return report_eol;
	}

	void GetToken(Token& out);
	void GetDelimitedString(Token& out, const std::function<bool(char)>& match_delim);

	template <typename T> T GetNumber() {
		GetToken(internalToken);
		return internalToken.GetNumericValue<T>();
	}

	std::string& GetIdentifier();
	std::string& GetStringLiteral();
	std::string& GetSpaceDelimitedString();

	void AssertIdentifier(const std::string& id);
	void AssertPunctuator(const std::string& punc);
	void AssertLabel(const std::string& label);

	void AssertEndOfFile();

	inline const Text::Location& GetInternalTokenLocation() const {
		return internalToken.Location;
	}
};

}
}
