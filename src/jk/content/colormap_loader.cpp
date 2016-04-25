#include "colormap_loader.hpp"
#include "colormap.hpp"

gorc::fourcc const gorc::colormap_loader::type = "CMP"_4CC;

std::unique_ptr<gorc::asset> gorc::colormap_loader::deserialize(input_stream &is,
                                                                content_manager &,
                                                                asset_id,
                                                                service_registry const &) const
{
    binary_input_stream bis(is);
    return std::make_unique<colormap>(deserialization_constructor, bis);
}

namespace {
    std::vector<gorc::path> const asset_root_path = { "misc/cmp" };
}

std::vector<gorc::path> const& gorc::colormap_loader::get_prefixes() const
{
    return asset_root_path;
}

gorc::maybe<char const *> gorc::colormap_loader::get_default() const
{
    return "dflt.cmp";
}
