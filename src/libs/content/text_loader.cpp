#include "text_loader.hpp"

std::unique_ptr<gorc::asset> gorc::text_loader::deserialize(input_stream &file,
                                                            content_manager &manager,
                                                            asset_id,
                                                            service_registry const &svc) const
{
    generic_tokenizer tok(file);
    return parse(tok, manager, svc);
}
