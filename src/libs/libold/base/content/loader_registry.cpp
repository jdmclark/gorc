#include "loader_registry.hpp"
#include "log/log.hpp"

gorc::content::loader const& gorc::loader_registry::get_loader(fourcc f)
{
    auto it = loaders.find(f);
    if(it == loaders.end()) {
        LOG_FATAL(format("no loader for content type %s") % to_string(f));
    }

    return *it->second;
}
