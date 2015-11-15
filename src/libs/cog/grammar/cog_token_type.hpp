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
            punctuator
        };

    }
}
