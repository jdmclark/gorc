#pragma once

#include "object_file_entity.hpp"

namespace gorc {

    class library_file_entity_internal_properties {
    protected:
        std::string library_name;
        std::unordered_set<object_file_entity*> objects;
        std::unordered_set<entity*> dependencies_value;

        library_file_entity_internal_properties(entity_input_stream &);
        library_file_entity_internal_properties(std::string const &library_name,
                                                std::unordered_set<object_file_entity*> const &obs);
        virtual ~library_file_entity_internal_properties();
    };

    class library_file_entity : private library_file_entity_internal_properties
                              , public base_file_entity {
    public:
        library_file_entity(entity_input_stream &);
        library_file_entity(std::string const &library_name,
                            std::unordered_set<object_file_entity*> const &objects,
                            service_registry const &);

        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool is_dirty() override;
        virtual bool update(service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;
    };

}
