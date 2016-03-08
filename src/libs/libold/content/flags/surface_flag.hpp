#pragma once

namespace gorc {
namespace flags {

enum class surface_flag {
    Floor                            = 0x1,
    CogLinked                        = 0x2,
    Impassable                         = 0x4,
    AiCannotWalkOnFloor                = 0x8,
    DoubletextureScale                = 0x10,
    HalftextureScale                = 0x20,
    EighthtextureScale                = 0x40,
    NoFallingDamage                    = 0x80,
    HorizonSky                        = 0x200,
    CeilingSky                        = 0x400,
    Scrolling                        = 0x800,
    Icy                                = 0x1000,
    VeryIcy                            = 0x2000,
    MagSealed                        = 0x4000,
    Metal                            = 0x10000,
    DeepWater                        = 0x20000,
    ShallowWater                    = 0x40000,
    Dirt                            = 0x80000,
    VeryDeepWater                    = 0x100000
};

}
}
