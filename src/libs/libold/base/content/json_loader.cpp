#include "json_loader.hpp"
#include "text/json_input_stream.hpp"

using namespace gorc::content;

std::unique_ptr<gorc::asset> json_loader::deserialize(input_stream &file,
                                                      content_manager &content,
                                                      asset_id,
                                                      service_registry const &svc) const
{
    auto jis = json_input_stream(file);
    return parse(jis, content, svc);
}
