#include "json_loader.hpp"

std::unique_ptr<gorc::asset> gorc::json_loader::deserialize(input_stream &file,
                                                            content_manager &content,
                                                            asset_id,
                                                            service_registry const &svc,
                                                            std::string const &name) const
{
    auto jis = json_input_stream(file);
    return parse(jis, content, svc);
}
