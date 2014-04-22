#include "base/text/tokenizer.h"
#include "base/text/source.h"
#include "base/diagnostics/stored_report.h"
#include "base/diagnostics/stream_report.h"
#include "test/test.h"

using namespace gorc;

begin_suite(TokenizerTest);

test_case(GetTokenTests) {
    // Basic tests of get_token and all token types.

    // Set up source:
    std::string input = " \
        # Line comment                                            \n\
        \"string literal\"                                        \n\
        \"string literal with \\\"escape sequences\\\"\"        \n\
        identifier                                                \n\
        numeric_0123456789_id                                    \n\
        _identifierwithleadingunderscore                        \n\
        9illegalidentifier                                        \n\
        illegal!identifier                                        \n\
        { } [ ] ( ) .                                             \n\
        & * + - ~ ! /                                             \n\
        % < > ^ | ? =                                            \n\
        , ; :                                                    \n\
        255 -255 0x255 0xFF -0x255 -0xFF 0XFF -0XFF                \n\
        1.0 -1.0 0.1 -0.1                                        \n\
        # Exponent formats                                        \n\
        1e2 -1e2 1E2 -1E2                                        \n\
        0.1e2 -0.1e2 0.1E2 -0.1E2                                \n\
        1e+2 1e-2 1E+2 1E-2                                        \n\
        # Invalid floating point format                            \n\
        .5                                                        \n\
        ";

    diagnostics::stored_report report;
    text::source s(input);
    text::tokenizer tok(s, report);

    text::token t;

    tok.get_token(t);
    assert_eq(t.type, text::token_type::string);
    assert_eq(t.value, "string literal");
    assert_eq(t.location.first_line, 2UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::string);
    assert_eq(t.value, "string literal with \"escape sequences\"");
    assert_eq(t.location.first_line, 3UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "identifier");
    assert_eq(t.location.first_line, 4UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "numeric_0123456789_id");
    assert_eq(t.location.first_line, 5UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "_identifierwithleadingunderscore");
    assert_eq(t.location.first_line, 6UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::integer);
    assert_eq(t.value, "9");
    assert_eq(t.location.first_line, 7UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "illegalidentifier");
    assert_eq(t.location.first_line, 7UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "illegal");
    assert_eq(t.location.first_line, 8UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "!");
    assert_eq(t.location.first_line, 8UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "identifier");
    assert_eq(t.location.first_line, 8UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "{");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "}");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "[");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "]");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "(");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ")");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ".");
    assert_eq(t.location.first_line, 9UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "&");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "*");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "+");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "-");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "~");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "!");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "/");
    assert_eq(t.location.first_line, 10UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "%");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "<");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ">");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "^");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "|");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "?");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, "=");
    assert_eq(t.location.first_line, 11UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ",");
    assert_eq(t.location.first_line, 12UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ";");
    assert_eq(t.location.first_line, 12UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ":");
    assert_eq(t.location.first_line, 12UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::integer);
    assert_eq(t.value, "255");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::integer);
    assert_eq(t.value, "-255");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::hex_integer);
    assert_eq(t.value, "0x255");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::hex_integer);
    assert_eq(t.value, "0xFF");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::hex_integer);
    assert_eq(t.value, "-0x255");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::hex_integer);
    assert_eq(t.value, "-0xFF");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::hex_integer);
    assert_eq(t.value, "0XFF");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::hex_integer);
    assert_eq(t.value, "-0XFF");
    assert_eq(t.location.first_line, 13UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1.0");
    assert_eq(t.location.first_line, 14UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "-1.0");
    assert_eq(t.location.first_line, 14UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "0.1");
    assert_eq(t.location.first_line, 14UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "-0.1");
    assert_eq(t.location.first_line, 14UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1e2");
    assert_eq(t.location.first_line, 16UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "-1e2");
    assert_eq(t.location.first_line, 16UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1E2");
    assert_eq(t.location.first_line, 16UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "-1E2");
    assert_eq(t.location.first_line, 16UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "0.1e2");
    assert_eq(t.location.first_line, 17UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "-0.1e2");
    assert_eq(t.location.first_line, 17UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "0.1E2");
    assert_eq(t.location.first_line, 17UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "-0.1E2");
    assert_eq(t.location.first_line, 17UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1e+2");
    assert_eq(t.location.first_line, 18UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1e-2");
    assert_eq(t.location.first_line, 18UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1E+2");
    assert_eq(t.location.first_line, 18UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::floating);
    assert_eq(t.value, "1E-2");
    assert_eq(t.location.first_line, 18UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::punctuator);
    assert_eq(t.value, ".");
    assert_eq(t.location.first_line, 20UL);

    tok.get_token(t);
    assert_eq(t.type, text::token_type::integer);
    assert_eq(t.value, "5");
    assert_eq(t.location.first_line, 20UL);
}

test_case(AssertionTests) {
    // Set up source:
    std::string input = " \
        identifier!                \n\
        label:                    \n\
        some~delimited~string.  \n\
        0xFF 255                \n\
        some:corrupted_data        \n\
        nice_data                \n\
        ";

    diagnostics::stream_report report(std::cout);
    text::source s(input);
    text::tokenizer tok(s, report);

    tok.assert_identifier("identifier");
    tok.assert_punctuator("!");
    tok.assert_label("label");

    text::token t;

    tok.get_delimited_string(t, [](char c) { return c == '.'; });
    assert_eq(t.type, text::token_type::string);
    assert_eq(t.value, "some~delimited~string");
    assert_eq(t.location.first_line, 3UL);

    tok.assert_punctuator(".");

    assert_eq(tok.get_number<int>(), 255);
    assert_eq(tok.get_number<int>(), 255);

    tok.assert_label("some");
    tok.skip_to_next_line();

    tok.get_token(t);
    assert_eq(t.type, text::token_type::identifier);
    assert_eq(t.value, "nice_data");
    assert_eq(t.location.first_line, 6UL);
}

test_case(ParseVectorTest) {
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

end_suite(TokenizerTest);
