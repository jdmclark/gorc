#include "colormap_loader.hpp"
#include "colormap.hpp"

std::unique_ptr<gorc::asset> gorc::colormap_loader::deserialize(input_stream &is,
                                                                content_manager &,
                                                                service_registry const &) const
{
    binary_input_stream bis(is);
    return std::make_unique<colormap>(deserialization_constructor, bis);
}
