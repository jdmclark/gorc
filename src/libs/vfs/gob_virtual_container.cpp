#include "gob_virtual_container.hpp"
#include "io/native_file.hpp"
#include "log/log.hpp"

namespace {
    struct gob_header {
        char magic[4];
        uint32_t first_index_offset;
        uint32_t unknown;
        uint32_t index_count;
    };

    struct gob_entry {
        uint32_t chunk_offset;
        uint32_t chunk_length;
        char chunk_name[128];
    };
}

gorc::gob_virtual_container::gob_virtual_container(path const &container_filename)
    : container_filename(container_filename)
{
    std::string stk_filename = container_filename.generic_string();
    diagnostic_context dc(stk_filename.c_str());

    auto file = make_native_read_only_file(container_filename);

    gob_header header;
    file->read(&header, sizeof(gob_header));

    if(strncmp(header.magic, "GOB ", 4) != 0) {
        LOG_FATAL("container is not a valid GOB");
    }

    gob_entry entry;
    for(uint32_t i = 0; i < header.index_count; ++i) {
        file->read(&entry, sizeof(gob_entry));

        // Convert path separators for boost path.
        std::replace_if(entry.chunk_name,
                        entry.chunk_name + 128,
                        [](char ch) { return ch == '\\'; },
                        '/');

        // Convert entry to lowercase.
        std::transform(entry.chunk_name,
                       entry.chunk_name + 128,
                       entry.chunk_name,
                       tolower);

        entry.chunk_name[127] = '\0';

        files.emplace_back(entry.chunk_name, *this, entry.chunk_offset, entry.chunk_length);
    }

    return;
}

size_t gorc::gob_virtual_container::size() const
{
    return files.size();
}

gorc::gob_virtual_file const & gorc::gob_virtual_container::get_file(size_t index) const
{
    return files[index];
}
