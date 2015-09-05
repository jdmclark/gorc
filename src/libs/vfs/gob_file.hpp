#pragma once

#include "io/input_stream.hpp"
#include "io/native_file.hpp"
#include <string>

namespace gorc {

    class gob_file : public input_stream {
    private:
        native_read_only_file file;
        size_t const chunk_offset;
        size_t const chunk_length;
        size_t const chunk_end;

    public:
        gob_file(path const &container_filename,
                 size_t chunk_offset,
                 size_t chunk_length);

        virtual size_t read_some(void *dest, size_t size) override;
        virtual bool at_end() override;
    };
}
