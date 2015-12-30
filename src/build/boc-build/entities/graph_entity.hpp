#pragma once

#include "build/boc-build/engine/entity.hpp"

namespace gorc {

    class graph_entity : public entity {
    private:
        std::unordered_set<entity*> project_files;

    public:
        graph_entity(entity_input_stream &);
        graph_entity(std::unordered_set<entity*> const &project_files);

        virtual std::string const& name() override;
        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool is_dirty() override;
        virtual bool update(gorc::service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;
    };

}
