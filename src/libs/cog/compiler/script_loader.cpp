#include "script_loader.hpp"
#include "compiler.hpp"

gorc::fourcc const gorc::cog::script_loader::type = "COG"_4CC;

std::unique_ptr<gorc::asset> gorc::cog::script_loader::deserialize(
        input_stream &is,
        content_manager &,
        service_registry const &services) const
{
    auto &compiler = services.get<cog::compiler>();
    return compiler.compile(is);
}
