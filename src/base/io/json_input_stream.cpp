#define __STDC_FORMAT_MACROS
#include "json_input_stream.h"
#include <inttypes.h>

gorc::io::json_input_stream::json_input_stream(input_stream& f) : f(f) {
    advance_char();
    advance_token();
    return;
}

void gorc::io::json_input_stream::advance_token() {
    curr_token.clear();

    // Skip whitespace
    while(isspace(next_ch)) {
        advance_char();
    }

    // Abort if at end
    if(!next_ch) {
        return;
    }

    // Add token
    char ch = next();
    switch(ch) {
    case '{':
        curr_token = "{";
        return;

    case '}':
        curr_token = "}";
        return;

    case '[':
        curr_token = "[";
        return;

    case ']':
        curr_token = "]";
        return;

    case ',':
        curr_token = ",";
        return;

    case ':':
        curr_token = ":";
        return;

    case '\"':
        while(true) {
            if(!next_ch) {
                // File terminates inside string literal
                throw json_invalid_format_exception();
            }
            else {
                ch = next();

                switch(ch) {
                default:
                    curr_token.push_back(ch);
                    break;

                case '\"':
                    return;

                case '\\':
                    if(!next_ch) {
                        // File terminates inside escape sequence
                        throw json_invalid_format_exception();
                    }

                    ch = next();

                    switch(ch) {
                    case '\\':
                    case '\"':
                        curr_token.push_back(ch);
                        break;

                    case '/':
                        curr_token.push_back('/');
                        break;

                    case 'b':
                        curr_token.push_back('\b');
                        break;

                    case 'f':
                        curr_token.push_back('\f');
                        break;

                    case 'n':
                        curr_token.push_back('\n');
                        break;

                    case 'r':
                        curr_token.push_back('\r');
                        break;

                    case 't':
                        curr_token.push_back('\t');
                        break;

                    default:
                        // Unknown escape sequence
                        throw json_invalid_format_exception();
                    }
                    break;
                }
            }
        }
        break;

    default:
        if(isalpha(ch) || ch == '_') {
            // Identifier
            curr_token.push_back(ch);
            while(isalnum(next_ch) || next_ch == '_') {
                curr_token.push_back(next());
            }

            // Check that token is valid
            if(curr_token != "true" && curr_token != "false" && curr_token != "null") {
                // Not a valid identifier token
                throw json_invalid_format_exception();
            }
        }
        else if(isdigit(ch) || ch == '-' || ch == '+') {
            // Must be a number.
            if(ch == '-' || ch == '+') {
                curr_token.push_back(ch);
                ch = next();
            }

            if(!isdigit(ch)) {
                throw json_invalid_format_exception();
            }
            else {
                curr_token.push_back(ch);
                while(isdigit(next_ch)) {
                    curr_token.push_back(next());
                }

                if(next_ch == '.') {
                    curr_token.push_back(next());

                    while(isdigit(next_ch)) {
                        curr_token.push_back(next());
                    }
                }

                if(next_ch == 'e' || next_ch == 'E') {
                    curr_token.push_back(next());

                    if(next_ch == '+' || next_ch == '-') {
                        curr_token.push_back(next());
                    }

                    while(isdigit(next_ch)) {
                        curr_token.push_back(next());
                    }
                }
            }
        }
        else {
            // Unknown char.
            throw json_invalid_format_exception();
        }
        break;
    }
}

int64_t gorc::io::json_input_stream::read_integer() {
    int64_t v;
    size_t result = sscanf(curr_token.c_str(), "%1024" PRId64, &v);

    if(result != 1) {
        throw json_invalid_format_exception();
    }

    return static_cast<int64_t>(v);
}

double gorc::io::json_input_stream::read_double() {
    double v;
    size_t result = sscanf(curr_token.c_str(), "%1024lf", &v);

    if(result != 1) {
        throw json_invalid_format_exception();
    }

    return v;
}
