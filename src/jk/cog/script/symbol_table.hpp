#pragma once

#include "symbol.hpp"
#include "utility/maybe.hpp"
#include <tuple>
#include <unordered_map>
#include <vector>

namespace gorc {
    namespace cog {
        enum class symbol_scope { global_symbol, local_symbol };

        class symbol_table {
        private:
            std::vector<symbol> global_symbols;
            std::unordered_map<std::string, size_t> global_index;

            std::vector<symbol> symbols;
            std::unordered_map<std::string, size_t> index;

            size_t internal_add_symbol(symbol &&);

        public:
            symbol_table();

            template <typename... ArgT>
            void add_symbol(ArgT &&... args)
            {
                internal_add_symbol(symbol(symbols.size(), std::forward<ArgT>(args)...));
            }

            std::tuple<symbol_scope, size_t> get_symbol_index(std::string const &name);
            std::tuple<symbol_scope, symbol const *> get_symbol(std::string const &name);

            std::tuple<symbol_scope, symbol const *> get_symbol_by_index(symbol_scope scope,
                                                                         size_t index) const;

            maybe<std::tuple<symbol_scope, size_t>>
                get_existing_symbol_index(std::string const &name) const;
            maybe<std::tuple<symbol_scope, symbol const *>>
                get_existing_symbol(std::string const &name) const;

            size_t size() const;

            inline auto begin() const -> decltype(symbols.begin())
            {
                return symbols.begin();
            }

            inline auto end() const -> decltype(symbols.end())
            {
                return symbols.end();
            }
        };
    }
}
