#pragma once

#include "collect_labels.hpp"
#include "cog/script/script.hpp"

namespace gorc {
    namespace cog {

        class collect_symbols_visitor {
        private:
            script &output_script;
            seen_label_set const &seen_labels;

            value_type get_type(ast::symbol &symbol);
            value get_value(ast::symbol &symbol, value_type vt);

        public:
            collect_symbols_visitor(script &output_script,
                                    seen_label_set const &seen_labels);

            void visit(ast::list_node<ast::symbol*> &);
            void visit(ast::symbol &);
        };

    }
}
