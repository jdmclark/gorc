#pragma once

#include "tokenizer.hpp"
#include "log/log.hpp"
#include "utility/string_view.hpp"
#include "utility/string_search.hpp"

namespace gorc {

    void assert_identifier(tokenizer &t, std::string const &ident);
    void assert_punctuator(tokenizer &t, std::string const &punct);

    template <typename T>
    T get_number(tokenizer &t)
    {
        bool format_success = false;
        T result = T(0);
        int64_t int_scratch;
        double dbl_scratch;

        std::stringstream ss;
        if(t.get_type() == token_type::punctuator) {
            if(iequal(t.get_value(), "+"_sv) ||
               iequal(t.get_value(), "-"_sv)) {
                ss << t.get_value();
                t.advance();
            }
        }

        ss << t.get_value();

        switch(t.get_type()) {
        case token_type::integer:
            // Uncomplicated
            format_success = !(ss >> result).fail();
            break;

        case token_type::hex_integer:
            // Hex ints must be read into an integer type
            format_success = !(ss >> std::hex >> int_scratch).fail();
            result = static_cast<T>(int_scratch);
            break;

        case token_type::oct_integer:
            // Oct ints must be read into an integer type
            format_success = !(ss >> std::oct >> int_scratch).fail();
            result = static_cast<T>(int_scratch);
            break;

        case token_type::real:
            // Reals must be read into a floating point type
            format_success = !(ss >> dbl_scratch).fail();
            result = static_cast<T>(dbl_scratch);
            break;

        default:
            format_success = false;
            break;
        }

        if(!format_success) {
            diagnostic_context dc(t.get_location());
            LOG_FATAL(format("expected a numeric value, found '%s'") % t.get_value());
        }

        t.advance();

        return result;
    }
}
