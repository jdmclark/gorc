#pragma once

#include "build/boc-build/engine/entity.hpp"
#include "io/path.hpp"

namespace gorc {

    class generated_file_entity : public entity {
    protected:
        std::string const entity_name;
        path const filename;

    public:
        generated_file_entity(path const &filename);

        virtual std::string const& name() override;
        virtual path const& file_path();

        virtual bool is_dirty() override;
    };

}
