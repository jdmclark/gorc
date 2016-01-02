#pragma once

#include "build/boc-build/engine/base_file_entity.hpp"
#include "io/path.hpp"

namespace gorc {

    class file_entity : public base_file_entity {
    protected:
        std::time_t previous_timestamp;

    public:
        file_entity(entity_input_stream &);
        file_entity(path const &filename,
                    std::time_t previous_timestamp = 0);

        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool is_dirty() override;
        virtual bool update(gorc::service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;

        virtual std::type_index get_type_index() const override;
    };

}
