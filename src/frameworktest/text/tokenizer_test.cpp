#include "framework/text/tokenizer.h"
#include "framework/text/source.h"
#include "framework/diagnostics/storedreport.h"
#include "framework/diagnostics/streamreport.h"
#include <nullunit/nullunit.h>

using namespace Gorc;

BeginSuite(TokenizerTest);

Case(GetTokenTests) {
	// Basic tests of GetToken and all token types.

	// Set up source:
	std::string input = " \
		# Line comment											\n\
		\"String literal\"										\n\
		\"String literal with \\\"escape sequences\\\"\"		\n\
		identifier												\n\
		numeric_0123456789_id									\n\
		_identifierwithleadingunderscore						\n\
		9illegalidentifier										\n\
		illegal!identifier										\n\
		{ } [ ] ( ) . 											\n\
		& * + - ~ ! / 											\n\
		% < > ^ | ? =											\n\
		, ; :													\n\
		255 -255 0x255 0xFF -0x255 -0xFF 0XFF -0XFF				\n\
		1.0 -1.0 0.1 -0.1										\n\
		# Exponent formats										\n\
		1e2 -1e2 1E2 -1E2										\n\
		0.1e2 -0.1e2 0.1E2 -0.1E2								\n\
		1e+2 1e-2 1E+2 1E-2										\n\
		# Invalid floating point format							\n\
		.5														\n\
		";

	Diagnostics::StoredReport report;
	Text::Source s(input);
	Text::Tokenizer tok(s, report);

	Text::Token t;

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::String);
	Test_Assert_Eq(t.Value, "String literal");
	Test_Assert_Eq(t.Location.first_line, 2);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::String);
	Test_Assert_Eq(t.Value, "String literal with \"escape sequences\"");
	Test_Assert_Eq(t.Location.first_line, 3);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "identifier");
	Test_Assert_Eq(t.Location.first_line, 4);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "numeric_0123456789_id");
	Test_Assert_Eq(t.Location.first_line, 5);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "_identifierwithleadingunderscore");
	Test_Assert_Eq(t.Location.first_line, 6);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Integer);
	Test_Assert_Eq(t.Value, "9");
	Test_Assert_Eq(t.Location.first_line, 7);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "illegalidentifier");
	Test_Assert_Eq(t.Location.first_line, 7);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "illegal");
	Test_Assert_Eq(t.Location.first_line, 8);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "!");
	Test_Assert_Eq(t.Location.first_line, 8);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "identifier");
	Test_Assert_Eq(t.Location.first_line, 8);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "{");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "}");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "[");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "]");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "(");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ")");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ".");
	Test_Assert_Eq(t.Location.first_line, 9);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "&");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "*");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "+");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "-");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "~");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "!");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "/");
	Test_Assert_Eq(t.Location.first_line, 10);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "%");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "<");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ">");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "^");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "|");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "?");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, "=");
	Test_Assert_Eq(t.Location.first_line, 11);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ",");
	Test_Assert_Eq(t.Location.first_line, 12);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ";");
	Test_Assert_Eq(t.Location.first_line, 12);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ":");
	Test_Assert_Eq(t.Location.first_line, 12);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Integer);
	Test_Assert_Eq(t.Value, "255");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Integer);
	Test_Assert_Eq(t.Value, "-255");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::HexInteger);
	Test_Assert_Eq(t.Value, "0x255");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::HexInteger);
	Test_Assert_Eq(t.Value, "0xFF");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::HexInteger);
	Test_Assert_Eq(t.Value, "-0x255");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::HexInteger);
	Test_Assert_Eq(t.Value, "-0xFF");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::HexInteger);
	Test_Assert_Eq(t.Value, "0XFF");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::HexInteger);
	Test_Assert_Eq(t.Value, "-0XFF");
	Test_Assert_Eq(t.Location.first_line, 13);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1.0");
	Test_Assert_Eq(t.Location.first_line, 14);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "-1.0");
	Test_Assert_Eq(t.Location.first_line, 14);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "0.1");
	Test_Assert_Eq(t.Location.first_line, 14);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "-0.1");
	Test_Assert_Eq(t.Location.first_line, 14);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1e2");
	Test_Assert_Eq(t.Location.first_line, 16);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "-1e2");
	Test_Assert_Eq(t.Location.first_line, 16);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1E2");
	Test_Assert_Eq(t.Location.first_line, 16);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "-1E2");
	Test_Assert_Eq(t.Location.first_line, 16);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "0.1e2");
	Test_Assert_Eq(t.Location.first_line, 17);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "-0.1e2");
	Test_Assert_Eq(t.Location.first_line, 17);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "0.1E2");
	Test_Assert_Eq(t.Location.first_line, 17);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "-0.1E2");
	Test_Assert_Eq(t.Location.first_line, 17);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1e+2");
	Test_Assert_Eq(t.Location.first_line, 18);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1e-2");
	Test_Assert_Eq(t.Location.first_line, 18);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1E+2");
	Test_Assert_Eq(t.Location.first_line, 18);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Float);
	Test_Assert_Eq(t.Value, "1E-2");
	Test_Assert_Eq(t.Location.first_line, 18);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Punctuator);
	Test_Assert_Eq(t.Value, ".");
	Test_Assert_Eq(t.Location.first_line, 20);

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Integer);
	Test_Assert_Eq(t.Value, "5");
	Test_Assert_Eq(t.Location.first_line, 20);
}

Case(AssertionTests) {
	// Set up source:
	std::string input = " \
		identifier!				\n\
		label:					\n\
		some~delimited~string.  \n\
		0xFF 255				\n\
		some:corrupted_data		\n\
		nice_data				\n\
		";

	Diagnostics::StreamReport report(std::cout);
	Text::Source s(input);
	Text::Tokenizer tok(s, report);

	tok.AssertIdentifier("identifier");
	tok.AssertPunctuator("!");
	tok.AssertLabel("label");

	Text::Token t;

	tok.GetDelimitedString(t, [](char c) { return c == '.'; });
	Test_Assert_Eq(t.Type, Text::TokenType::String);
	Test_Assert_Eq(t.Value, "some~delimited~string");
	Test_Assert_Eq(t.Location.first_line, 3);

	tok.AssertPunctuator(".");

	Test_Assert_Eq(tok.GetNumber<int>(), 255);
	Test_Assert_Eq(tok.GetNumber<int>(), 255);

	tok.AssertLabel("some");
	tok.SkipToNextLine();

	tok.GetToken(t);
	Test_Assert_Eq(t.Type, Text::TokenType::Identifier);
	Test_Assert_Eq(t.Value, "nice_data");
	Test_Assert_Eq(t.Location.first_line, 6);
}

Case(ParseVectorTest) {
	std::string input = " \
			frame=(-8.850002/-5.300000/-0.671531:0.000000/-90.000000/0.000000)";
	Diagnostics::StreamReport report(std::cout);
	Text::Source s(input);
	Text::Tokenizer tok(s, report);

	tok.AssertIdentifier("frame");
	tok.AssertPunctuator("=");

	tok.AssertPunctuator("(");
	tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	tok.GetNumber<float>();
	tok.AssertPunctuator(":");
	tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	tok.GetNumber<float>();
	tok.AssertPunctuator(")");
}

EndSuite(TokenizerTest);
