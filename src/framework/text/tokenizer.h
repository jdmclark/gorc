#pragma once

#include <string>
#include <sstream>
#include "source.h"
#include "token.h"
#include "framework/diagnostics/report.h"
#include "framework/diagnostics/errorlocation.h"

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

	inline const std::string& GetFilename() const {
		return buffer.Filename;
	}

	void GetToken(Token& out);
	void GetDelimitedString(Token& out, char delimiter = ' ');

	template <typename T> T GetNumber() {
		GetToken(internalToken);
		return internalToken.GetNumericValue<T>();
	}

	void AssertIdentifier(const std::string& id);
	void AssertPunctuator(const std::string& punc);
	void AssertLabel(const std::string& label);

	void AssertEndOfFile();
};

}
}
