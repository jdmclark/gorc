#pragma once

#include "value.hpp"
#include "type.hpp"
#include "mock_verb.hpp"
#include "function_verb.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace gorc {
    namespace cog {

        class verb_table {
        private:
            std::vector<std::unique_ptr<verb>> verbs;
            std::unordered_map<std::string, std::tuple<int, bool>> verb_index;

            void internal_add_verb(std::unique_ptr<verb> &&v);

        public:
            template <typename FnT>
            void add_verb(std::string const &name, FnT functor)
            {
                internal_add_verb(make_function_verb(name, functor));
            }

            template <typename FnT>
            void add_service_verb(std::string const &name, FnT functor)
            {
                internal_add_verb(make_service_verb(name, functor));
            }

            template <typename VerbT, typename ...ArgT>
            void emplace_verb(ArgT ...args)
            {
                internal_add_verb(std::make_unique<VerbT>(std::forward<ArgT>(args)...));
            }

            void add_synonym(std::string const &name, std::string const &syn);

            void add_deprecation(std::string const &name);

            verb_id get_verb_id(std::string const &name) const;
            verb const& get_verb(verb_id) const;
        };

    }
}
