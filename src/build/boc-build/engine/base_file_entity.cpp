#include "base_file_entity.hpp"

gorc::base_file_entity::base_file_entity(path const &new_filename)
    : entity_name(new_filename.generic_string())
    , filename(new_filename)
{
    return;
}

std::string const& gorc::base_file_entity::name()
{
    return entity_name;
}

gorc::path const& gorc::base_file_entity::file_path()
{
    return filename;
}
