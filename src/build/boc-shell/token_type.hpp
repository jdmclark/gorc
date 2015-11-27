#pragma once

namespace gorc {

    enum class shell_token_type {
        error,
        end_of_file,
        word,

        punc_end_command
    };

}
