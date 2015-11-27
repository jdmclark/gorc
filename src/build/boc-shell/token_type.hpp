#pragma once

namespace gorc {

    enum class shell_token_type {
        error,
        end_of_file,
        first_word,
        successor_word,

        punc_pipe,
        punc_end_command
    };

}
