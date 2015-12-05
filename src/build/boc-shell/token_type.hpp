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

        kw_include,
        kw_var
    };

}
