#include "generated_file_entity.hpp"
#include <boost/filesystem.hpp>

gorc::generated_file_entity::generated_file_entity(path const &new_filename)
    : entity_name(new_filename.generic_string())
    , filename(new_filename)
{
    return;
}

std::string const& gorc::generated_file_entity::name()
{
    return entity_name;
}

gorc::path const& gorc::generated_file_entity::file_path()
{
    return filename;
}

bool gorc::generated_file_entity::is_dirty()
{
    return last_update_failed ||
           !boost::filesystem::is_regular_file(filename);
}
