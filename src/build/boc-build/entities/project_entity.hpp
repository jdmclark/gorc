#pragma once

#include "program_file_entity.hpp"

namespace gorc {

    class project_entity : public entity {
    private:
        std::unordered_set<program_file_entity*> programs;
        std::unordered_set<entity*> dependencies_value;

    public:
        project_entity(entity_input_stream &);
        project_entity(std::unordered_set<program_file_entity*> const &programs);

        virtual std::string const& name() override;
        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool is_dirty() override;
        virtual bool update(service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;

        virtual std::type_index get_type_index() const override;

        std::unordered_set<program_file_entity*> const& get_programs() const;
    };

}
