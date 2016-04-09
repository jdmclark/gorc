#include "script_loader.hpp"
#include "compiler.hpp"

gorc::fourcc const gorc::cog::script_loader::type = "COG"_4CC;

namespace {
    std::vector<gorc::path> cog_prefixes = {
        "cog"
    };
}

std::vector<gorc::path> const& gorc::cog::script_loader::get_prefixes() const
{
    return cog_prefixes;
}

std::unique_ptr<gorc::asset> gorc::cog::script_loader::deserialize(
        input_stream &is,
        content_manager &,
        service_registry const &services) const
{
    auto &compiler = services.get<cog::compiler>();
    return compiler.compile(is);
}
