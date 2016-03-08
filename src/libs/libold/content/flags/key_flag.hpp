#pragma once

namespace gorc {
namespace flags {

enum class key_flag {
    PausesOnFirstFrame         = 0x1,
    DoesNotLoop                = 0x2,
    PausesOnLastFrame        = 0x4,
    RestartIfPlaying        = 0x8,
    FinishInGivenTime        = 0x10,
    EndSmoothly                = 0x20
};

}
}
