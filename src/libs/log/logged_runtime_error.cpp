#include "logged_runtime_error.hpp"

gorc::logged_runtime_error::logged_runtime_error()
    : std::runtime_error("")
{
    return;
}
