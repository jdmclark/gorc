#pragma once

#include "options.hpp"
#include "abstract_argument_queue.hpp"
#include "subcommand.hpp"
#include <unordered_map>
#include <string>

namespace gorc {

    class subcommand_registry {
    private:
        std::unordered_map<std::string, subcommand*> subcommands;

    public:
        void add_subcommand(std::string const &name, subcommand &sub);
        int start(std::string const &name, abstract_argument_queue &);
    };

}
