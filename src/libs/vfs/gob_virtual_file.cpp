#include "gob_virtual_file.hpp"
#include "gob_virtual_container.hpp"
#include "gob_file.hpp"
#include "utility/make_unique.hpp"

gorc::gob_virtual_file::gob_virtual_file(path const &name,
                                         gob_virtual_container const &parent_container,
                                         size_t chunk_offset,
                                         size_t chunk_length)
    : virtual_file(name)
    , parent_container(parent_container)
    , chunk_offset(chunk_offset)
    , chunk_length(chunk_length)
{
    return;
}

std::unique_ptr<gorc::input_stream> gorc::gob_virtual_file::open() const
{
    return make_unique<gob_file>(parent_container.container_filename,
                                 chunk_offset,
                                 chunk_length);
}

gorc::virtual_container const& gorc::gob_virtual_file::get_parent_container() const
{
    return parent_container;
}
