#pragma once

namespace gorc {
    namespace cog {

        enum class cog_token_type {
            error,
            end_of_file,
            end_of_line,
            identifier,
            string,
            integer,
            hex_integer,
            real,

            // Punctuators
            punc_equal,
            punc_notequal,
            punc_greater,
            punc_greaterequal,
            punc_less,
            punc_lessequal,
            punc_logical_and,
            punc_logical_or,
            punc_colon,
            punc_semicolon,
            punc_comma,
            punc_apos,
            punc_plus,
            punc_minus,
            punc_times,
            punc_div,
            punc_mod,
            punc_excl,
            punc_and,
            punc_or,
            punc_xor,
            punc_assign,
            punc_lbrace,
            punc_rbrace,
            punc_lparen,
            punc_rparen,
            punc_lbracket,
            punc_rbracket
        };

    }
}
