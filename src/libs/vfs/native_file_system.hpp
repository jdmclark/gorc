#pragma once

#include "virtual_file_system.hpp"

namespace gorc {

    class native_file_system : public virtual_file_system {
    public:
        virtual std::unique_ptr<input_stream> open(path const &filename) const override;
    };

}
