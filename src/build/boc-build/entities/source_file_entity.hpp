#pragma once

#include "file_entity.hpp"

namespace gorc {

    class source_file_entity : public file_entity {
    public:
        using file_entity::file_entity;

        virtual void serialize(entity_output_stream &) override;

        virtual std::type_index get_type_index() const override;
    };

}
