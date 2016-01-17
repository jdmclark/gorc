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
        punc_begin_subshell,
        punc_equal,
        punc_notequal,
        punc_not,
        punc_cons,
        punc_comma,
        punc_logical_or,
        punc_logical_and,
        punc_stdin_redirect,
        punc_stdout_redirect,
        punc_stdout_append,
        punc_stderr_redirect,
        punc_stderr_append,

        kw_call,
        kw_else,
        kw_for,
        kw_function,
        kw_if,
        kw_in,
        kw_include,
        kw_nil,
        kw_popd,
        kw_pushd,
        kw_return,
        kw_var
    };

}
