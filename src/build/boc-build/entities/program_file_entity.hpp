#pragma once

#include "object_file_entity.hpp"
#include "library_file_entity.hpp"
#include "program_type.hpp"
#include "generated_file_entity.hpp"

namespace gorc {

    class program_file_entity_internal_properties {
    protected:
        std::string program_name;
        program_type type;
        std::unordered_set<object_file_entity*> objects;
        std::unordered_set<library_file_entity*> libraries;

        std::unordered_set<entity*> dependencies_value;

        program_file_entity_internal_properties(entity_input_stream &);
        program_file_entity_internal_properties(std::string const &program_name,
                                                program_type type,
                                                std::unordered_set<object_file_entity*> const &obj,
                                                std::unordered_set<library_file_entity*> const &lib);
        virtual ~program_file_entity_internal_properties();
    };

    class program_file_entity : private program_file_entity_internal_properties
                              , public generated_file_entity {
    public:
        program_file_entity(entity_input_stream &);
        program_file_entity(std::string const &program_name,
                            program_type type,
                            std::unordered_set<object_file_entity*> const &objs,
                            std::unordered_set<library_file_entity*> const &libs,
                            service_registry const &);

        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool update(service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;

        std::unordered_set<object_file_entity*> const& get_objects() const;
        std::unordered_set<library_file_entity*> const& get_libraries() const;
    };
}
