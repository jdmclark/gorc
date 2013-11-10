#include "framework/text/tokenizer.h"
#include "framework/text/source.h"
#include "framework/diagnostics/stored_report.h"
#include "framework/diagnostics/stream_report.h"
#include <nullunit/nullunit.h>

using namespace gorc;

BeginSuite(TokenizerTest);

Case(GetTokenTests) {
	// Basic tests of get_token and all token types.

	// Set up source:
	std::string input = " \
		# Line comment											\n\
		\"string literal\"										\n\
		\"string literal with \\\"escape sequences\\\"\"		\n\
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

	diagnostics::stored_report report;
	text::source s(input);
	text::tokenizer tok(s, report);

	text::token t;

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::string);
	Test_Assert_Eq(t.value, "string literal");
	Test_Assert_Eq(t.location.first_line, 2);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::string);
	Test_Assert_Eq(t.value, "string literal with \"escape sequences\"");
	Test_Assert_Eq(t.location.first_line, 3);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "identifier");
	Test_Assert_Eq(t.location.first_line, 4);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "numeric_0123456789_id");
	Test_Assert_Eq(t.location.first_line, 5);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "_identifierwithleadingunderscore");
	Test_Assert_Eq(t.location.first_line, 6);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::integer);
	Test_Assert_Eq(t.value, "9");
	Test_Assert_Eq(t.location.first_line, 7);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "illegalidentifier");
	Test_Assert_Eq(t.location.first_line, 7);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "illegal");
	Test_Assert_Eq(t.location.first_line, 8);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "!");
	Test_Assert_Eq(t.location.first_line, 8);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "identifier");
	Test_Assert_Eq(t.location.first_line, 8);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "{");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "}");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "[");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "]");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "(");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ")");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ".");
	Test_Assert_Eq(t.location.first_line, 9);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "&");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "*");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "+");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "-");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "~");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "!");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "/");
	Test_Assert_Eq(t.location.first_line, 10);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "%");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "<");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ">");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "^");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "|");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "?");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, "=");
	Test_Assert_Eq(t.location.first_line, 11);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ",");
	Test_Assert_Eq(t.location.first_line, 12);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ";");
	Test_Assert_Eq(t.location.first_line, 12);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ":");
	Test_Assert_Eq(t.location.first_line, 12);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::integer);
	Test_Assert_Eq(t.value, "255");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::integer);
	Test_Assert_Eq(t.value, "-255");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::hex_integer);
	Test_Assert_Eq(t.value, "0x255");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::hex_integer);
	Test_Assert_Eq(t.value, "0xFF");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::hex_integer);
	Test_Assert_Eq(t.value, "-0x255");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::hex_integer);
	Test_Assert_Eq(t.value, "-0xFF");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::hex_integer);
	Test_Assert_Eq(t.value, "0XFF");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::hex_integer);
	Test_Assert_Eq(t.value, "-0XFF");
	Test_Assert_Eq(t.location.first_line, 13);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1.0");
	Test_Assert_Eq(t.location.first_line, 14);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "-1.0");
	Test_Assert_Eq(t.location.first_line, 14);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "0.1");
	Test_Assert_Eq(t.location.first_line, 14);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "-0.1");
	Test_Assert_Eq(t.location.first_line, 14);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1e2");
	Test_Assert_Eq(t.location.first_line, 16);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "-1e2");
	Test_Assert_Eq(t.location.first_line, 16);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1E2");
	Test_Assert_Eq(t.location.first_line, 16);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "-1E2");
	Test_Assert_Eq(t.location.first_line, 16);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "0.1e2");
	Test_Assert_Eq(t.location.first_line, 17);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "-0.1e2");
	Test_Assert_Eq(t.location.first_line, 17);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "0.1E2");
	Test_Assert_Eq(t.location.first_line, 17);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "-0.1E2");
	Test_Assert_Eq(t.location.first_line, 17);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1e+2");
	Test_Assert_Eq(t.location.first_line, 18);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1e-2");
	Test_Assert_Eq(t.location.first_line, 18);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1E+2");
	Test_Assert_Eq(t.location.first_line, 18);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::floating);
	Test_Assert_Eq(t.value, "1E-2");
	Test_Assert_Eq(t.location.first_line, 18);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::punctuator);
	Test_Assert_Eq(t.value, ".");
	Test_Assert_Eq(t.location.first_line, 20);

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::integer);
	Test_Assert_Eq(t.value, "5");
	Test_Assert_Eq(t.location.first_line, 20);
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

	diagnostics::stream_report report(std::cout);
	text::source s(input);
	text::tokenizer tok(s, report);

	tok.assert_identifier("identifier");
	tok.assert_punctuator("!");
	tok.assert_label("label");

	text::token t;

	tok.get_delimited_string(t, [](char c) { return c == '.'; });
	Test_Assert_Eq(t.type, text::token_type::string);
	Test_Assert_Eq(t.value, "some~delimited~string");
	Test_Assert_Eq(t.location.first_line, 3);

	tok.assert_punctuator(".");

	Test_Assert_Eq(tok.get_number<int>(), 255);
	Test_Assert_Eq(tok.get_number<int>(), 255);

	tok.assert_label("some");
	tok.skip_to_next_line();

	tok.get_token(t);
	Test_Assert_Eq(t.type, text::token_type::identifier);
	Test_Assert_Eq(t.value, "nice_data");
	Test_Assert_Eq(t.location.first_line, 6);
}

Case(ParseVectorTest) {
	std::string input = " \
			frame=(-8.850002/-5.300000/-0.671531:0.000000/-90.000000/0.000000)";
	diagnostics::stream_report report(std::cout);
	text::source s(input);
	text::tokenizer tok(s, report);

	tok.assert_identifier("frame");
	tok.assert_punctuator("=");

	tok.assert_punctuator("(");
	tok.get_number<float>();
	tok.assert_punctuator("/");
	tok.get_number<float>();
	tok.assert_punctuator("/");
	tok.get_number<float>();
	tok.assert_punctuator(":");
	tok.get_number<float>();
	tok.assert_punctuator("/");
	tok.get_number<float>();
	tok.assert_punctuator("/");
	tok.get_number<float>();
	tok.assert_punctuator(")");
}

EndSuite(TokenizerTest);
