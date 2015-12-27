#pragma once

#include "build/boc-build/engine/entity.hpp"
#include "io/path.hpp"

namespace gorc {

    class file_entity : public entity {
    private:
        std::string entity_name;
        path const filename;
        std::time_t previous_timestamp;

    public:
        file_entity(path const &filename,
                    std::time_t previous_timestamp = 0);

        virtual std::string const& name() override;
        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool is_dirty() override;
        virtual bool update(gorc::service_registry const &) override;
    };

}
