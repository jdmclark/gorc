#include "file_entity.hpp"
#include <boost/filesystem.hpp>

namespace {
    std::unordered_set<gorc::entity*> empty_dependencies;
}

gorc::file_entity::file_entity(path const &new_filename,
                               std::time_t previous_timestamp)
    : entity_name(new_filename.generic_string())
    , filename(new_filename)
    , previous_timestamp(previous_timestamp)
{
    return;
}

std::string const& gorc::file_entity::name()
{
    return entity_name;
}

std::unordered_set<gorc::entity*> const& gorc::file_entity::dependencies()
{
    return empty_dependencies;
}

bool gorc::file_entity::is_dirty()
{
    return previous_timestamp < boost::filesystem::last_write_time(filename);
}

bool gorc::file_entity::update(gorc::service_registry const &)
{
    previous_timestamp = boost::filesystem::last_write_time(filename);
    return true;
}
