#pragma once

#include "entity.hpp"
#include "io/path.hpp"

namespace gorc {

    class base_file_entity : public entity {
    protected:
        std::string const entity_name;
        path const filename;

    public:
        base_file_entity(path const &filename);

        virtual std::string const& name() override;
    };

}
