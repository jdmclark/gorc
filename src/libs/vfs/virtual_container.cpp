#include "virtual_container.hpp"

gorc::virtual_container::~virtual_container()
{
    return;
}

gorc::virtual_container_iterator gorc::virtual_container::begin() const
{
    return virtual_container_iterator(*this, 0);
}

gorc::virtual_container_iterator gorc::virtual_container::end() const
{
    return virtual_container_iterator(*this, size());
}
