#include "sleep_record.hpp"

gorc::cog::sleep_record::sleep_record(continuation &&cc,
                                      time_delta expiration_time)
    : cc(std::forward<continuation>(cc))
    , expiration_time(expiration_time)
{
    return;
}
