#pragma once

namespace gorc {
namespace flags {

enum class attach_flag {
    AttachedToWorldSurface            = 0x1,
    AttachedToThingFace                = 0x2,
    AttachedToThing                    = 0x4,
    NoMoveRelativeToAttachedThing    = 0x8
};


}
}
