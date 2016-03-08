#pragma once

#include <string>
#include <sstream>
#include <functional>
#include "io/input_stream.hpp"
#include "token.hpp"
#include "log/diagnostic_context_location.hpp"

namespace gorc {
namespace text {

class tokenizer {
private:
    char current, next;
    input_stream& buffer;
    diagnostic_context_location loc;
    int line, col;
    token internalToken;
    bool report_eol;

    inline char buffer_get_next();

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
    tokenizer(input_stream& stream);

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

    inline const diagnostic_context_location& get_internal_token_location() const {
        return internalToken.location;
    }
};

}
}
