#include "argument_visitor.hpp"
#include "word_visitor.hpp"
#include "utility/range.hpp"
#include "log/log.hpp"

gorc::shvalue gorc::argument_visitor::visit(argument &arg) const
{
    std::vector<shvalue> arg_parts;
    for(auto &part : arg.words->elements) {
        arg_parts.push_back(ast_visit(word_visitor(), *part));
    }

    std::vector<std::string> assembled_arg;

    for(auto const &value : arg_parts) {
        auto it = value.begin();

        // Merge the first value with the last from the previous fragment.
        if(!assembled_arg.empty() && it != value.end()) {
            assembled_arg.back() += *it;
            ++it;
        }

        for(; it != value.end(); ++it) {
            assembled_arg.push_back(*it);
        }
    }

    return assembled_arg;
}

gorc::shvalue gorc::argument_visitor::visit(ast_list_node<argument*> &args) const
{
    std::vector<shvalue> arg_list;
    for(auto &arg : args.elements) {
        arg_list.push_back(ast_visit(*this, arg));
    }

    shvalue rv;
    for(auto const &arg : arg_list) {
        std::copy(arg.begin(), arg.end(), std::back_inserter(rv));
    }

    return rv;
}
