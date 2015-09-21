#pragma once

#include "symbol.hpp"
#include "utility/maybe.hpp"
#include <unordered_map>
#include <vector>

namespace gorc {
    namespace cog {

        class symbol_table {
        private:
            std::vector<symbol> symbols;
            std::unordered_map<std::string, size_t> index;

            size_t internal_add_symbol(symbol &&);

        public:
            template <typename ...ArgT>
            void add_symbol(ArgT &&...args)
            {
                internal_add_symbol(symbol(symbols.size(), std::forward<ArgT>(args)...));
            }

            size_t get_symbol_index(std::string const &name);
            symbol const& get_symbol(std::string const &name);

            symbol const& get_symbol_by_index(size_t index) const;

            maybe<symbol const *> get_existing_symbol(std::string const &name) const;

            size_t size() const;

            inline auto begin() const
                -> decltype(symbols.begin())
            {
                return symbols.begin();
            }

            inline auto end() const
                -> decltype(symbols.end())
            {
                return symbols.end();
            }
        };

    }
}
