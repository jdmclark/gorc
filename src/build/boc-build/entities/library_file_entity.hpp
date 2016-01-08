#pragma once

#include "object_file_entity.hpp"
#include "generated_file_entity.hpp"
#include "external_lib_type.hpp"

namespace gorc {

    class library_file_entity;

    class library_file_entity_internal_properties {
    protected:
        std::string library_name;
        ext_lib_set external_libs;
        std::unordered_set<object_file_entity*> objects;
        std::unordered_set<library_file_entity*> libraries;
        std::unordered_set<entity*> dependencies_value;

        library_file_entity_internal_properties(entity_input_stream &);
        library_file_entity_internal_properties(std::string const &library_name,
                                                ext_lib_set const &ext_libs,
                                                std::unordered_set<object_file_entity*> const &obs,
                                                std::unordered_set<library_file_entity*> const &ls);
        virtual ~library_file_entity_internal_properties();
    };

    class library_file_entity : private library_file_entity_internal_properties
                              , public generated_file_entity {
    public:
        library_file_entity(entity_input_stream &);
        library_file_entity(std::string const &library_name,
                            ext_lib_set const &ext_libs,
                            std::unordered_set<object_file_entity*> const &objects,
                            std::unordered_set<library_file_entity*> const &libs,
                            service_registry const &);

        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool update(service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;

        ext_lib_set const& get_external_libraries() const;
        std::unordered_set<object_file_entity*> const& get_objects() const;
        std::unordered_set<library_file_entity*> const& get_libraries() const;

        std::string const& get_library_name() const;

        virtual std::type_index get_type_index() const override;
    };

}
