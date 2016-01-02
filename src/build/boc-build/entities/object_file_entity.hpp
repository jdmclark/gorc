#pragma once

#include "source_file_entity.hpp"
#include "generated_file_entity.hpp"

namespace gorc {

    class object_file_entity : public generated_file_entity {
    public:
        source_file_entity * const primary_source_file;

    private:
        std::unordered_set<entity*> dependencies_value;

    public:
        object_file_entity(entity_input_stream &);
        object_file_entity(source_file_entity *,
                           service_registry const &);

        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool update(service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;

        virtual std::type_index get_type_index() const override;
    };

}
