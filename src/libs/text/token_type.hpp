#pragma once

namespace gorc {
    enum class token_type {
        error,
        end_of_file,
        end_of_line,
        identifier,
        string,
        integer,
        hex_integer,
        oct_integer,
        real,
        punctuator
    };
}
