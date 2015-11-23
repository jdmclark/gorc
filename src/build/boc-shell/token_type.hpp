#pragma once

namespace gorc {

    enum class shell_token_type {
        error,
        end_of_file,
        word,
        variable,

        // Punctuators
        punc_lbrace,
        punc_rbrace,
        punc_lparen,
        punc_rparen,

        punc_glue,
        punc_assign,
        punc_semicolon,
        punc_pipe,
        punc_stream_trunc,
        punc_stream_concat
    };

}
