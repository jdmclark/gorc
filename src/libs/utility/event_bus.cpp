#include "event_bus.hpp"

gorc::abstract_delegate_container::~abstract_delegate_container()
{
    return;
}

gorc::scoped_delegate::scoped_delegate(abstract_delegate_container *observer,
                                       size_t id,
                                       bool is_const)
    : should_unregister(true)
    , observer(observer)
    , id(id)
    , is_const(is_const)
{
    return;
}

void gorc::scoped_delegate::unregister()
{
    if(should_unregister) {
        should_unregister = false;
        observer->unregister(id, is_const);
    }
}

gorc::scoped_delegate::scoped_delegate(scoped_delegate &&other)
    : should_unregister(true)
    , observer(other.observer)
    , id(other.id)
    , is_const(other.is_const)
{
    other.should_unregister = false;
    return;
}

gorc::scoped_delegate::~scoped_delegate()
{
    unregister();
    return;
}

gorc::scoped_delegate& gorc::scoped_delegate::operator=(scoped_delegate &&other)
{
    unregister();
    should_unregister = other.should_unregister;
    observer = other.observer;
    id = other.id;
    is_const = other.is_const;
    other.should_unregister = false;
    return *this;
}
