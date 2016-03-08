#include "text_loader.hpp"

std::unique_ptr<gorc::asset> gorc::content::text_loader::deserialize(input_stream &file,
                                                                     content_manager &manager,
                                                                     service_registry const &svc) const
{
    text::tokenizer tok(file);
    return parse(tok, manager, svc);
}
