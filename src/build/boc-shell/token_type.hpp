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

        kw_include,
        kw_var
    };

}
