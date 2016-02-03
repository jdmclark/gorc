#include "asset_ref.hpp"
#include "content_manager.hpp"

gorc::asset const& gorc::get_asset_from_id(service_registry const &services,
                                           size_t asset_numeric_id)
{
    return services.get<content_manager>().load_from_id(asset_numeric_id);
}
