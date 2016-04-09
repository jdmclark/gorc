#pragma once

#include "vfs/virtual_container.hpp"
#include "gob_virtual_file.hpp"
#include <vector>

namespace gorc {

    class gob_virtual_container : public virtual_container {
    private:
        std::vector<gob_virtual_file> files;

    public:
        gob_virtual_container(path const &container_filename);

        virtual gob_virtual_file const& get_file(size_t index) const override;
        virtual size_t size() const override;
    };

}
