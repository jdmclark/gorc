#include "episode_entry.hpp"

gorc::episode_entry::episode_entry(int number,
                                   int disc,
                                   int level_number,
                                   episode_entry_type type,
                                   path const &filename,
                                   int light_power,
                                   int dark_power,
                                   int goto_a,
                                   int goto_b)
    : number(number)
    , disc(disc)
    , level_number(level_number)
    , type(type)
    , filename(filename)
    , light_power(light_power)
    , dark_power(dark_power)
    , goto_a(goto_a)
    , goto_b(goto_b)
{
    return;
}
