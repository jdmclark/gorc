#pragma once

namespace gorc {
    struct uninit_constructor_tag { };
    static constexpr uninit_constructor_tag uninit_constructor = { };
}
