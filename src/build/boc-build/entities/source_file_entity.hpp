#pragma once

#include "file_entity.hpp"

namespace gorc {

    class source_file_entity : public file_entity {
    public:
        source_file_entity(entity_input_stream &);
        source_file_entity(path const &filename,
                           std::time_t previous_timestamp = 0);

        virtual void serialize(entity_output_stream &) override;

        virtual std::type_index get_type_index() const override;
    };

}
