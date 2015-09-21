#pragma once

namespace gorc {
    struct uninit_constructor_tag { };
    static constexpr uninit_constructor_tag uninit_constructor = { };

    struct deserialization_constructor_tag { };
    static constexpr deserialization_constructor_tag deserialization_constructor = { };
}
