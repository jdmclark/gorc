#pragma once

#include "loader.hpp"
#include "content/fourcc.hpp"
#include <unordered_map>
#include <memory>

namespace gorc {

    class loader_registry {
    private:
        std::unordered_map<fourcc, std::unique_ptr<gorc::content::loader>> loaders;

    public:
        template <typename T, typename ...ArgT>
        void emplace_loader(ArgT &&...arg)
        {
            loaders.emplace(T::type, std::make_unique<T>(std::forward<ArgT>(arg)...));
        }

        gorc::content::loader const& get_loader(fourcc f);
    };

}
