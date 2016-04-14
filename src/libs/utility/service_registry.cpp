#include "service_registry.hpp"

gorc::service_registry::service_registry(maybe<service_registry const*> parent)
    : parent(parent)
{
    return;
}
