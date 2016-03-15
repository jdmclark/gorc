#include "puppet_submode.hpp"

gorc::content::assets::puppet_submode::puppet_submode(asset_ref<animation> anim,
                                                      flag_set<flags::key_flag> flags,
                                                      int lo_priority,
                                                      int hi_priority)
    : anim(anim)
    , flags(flags)
    , lo_priority(lo_priority)
    , hi_priority(hi_priority)
{
    return;
}
