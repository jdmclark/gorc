#pragma once

namespace gorc {

    enum class shell_token_type {
        error,
        end_of_file,
        word,
        variable_name,
        environment_variable_name,

        punc_assign,
        punc_whitespace,
        punc_pipe,
        punc_end_command,
        punc_begin_block,
        punc_end_block,
        punc_begin_expr,
        punc_end_expr,
        punc_equal,
        punc_notequal,
        punc_not,
        punc_cons,
        punc_comma,

        kw_atom,
        kw_call,
        kw_car,
        kw_cdr,
        kw_else,
        kw_function,
        kw_if,
        kw_include,
        kw_nil,
        kw_null,
        kw_return,
        kw_var
    };

}
