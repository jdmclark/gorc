#pragma once

#include "value.hpp"
#include "type.hpp"
#include "utility/make_unique.hpp"
#include "mock_verb.hpp"
#include "function_verb.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace gorc {
    namespace cog {

        class verb_table {
        private:
            std::vector<std::unique_ptr<verb>> verbs;
            std::unordered_map<std::string, int> verb_index;

            void internal_add_verb(std::unique_ptr<verb> &&v);

        public:
            template <typename FnT>
            void add_verb(std::string const &name, FnT functor)
            {
                internal_add_verb(make_function_verb(name, functor));
            }

            template <typename VerbT, typename ...ArgT>
            void emplace_verb(ArgT ...args)
            {
                internal_add_verb(make_unique<VerbT>(std::forward<ArgT>(args)...));
            }

            void add_synonym(std::string const &name, std::string const &syn);

            verb_id get_verb_id(std::string const &name) const;
            verb const& get_verb(verb_id) const;
        };

    }
}
