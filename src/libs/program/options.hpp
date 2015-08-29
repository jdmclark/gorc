#pragma once

#include "range_argument_queue.hpp"
#include "switch_option.hpp"
#include "value_option.hpp"
#include "multi_value_option.hpp"
#include "mutual_exclusion.hpp"
#include "required_option.hpp"

#include "utility/make_unique.hpp"
#include "log/log.hpp"
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace gorc {

    class options {
    private:
        std::vector<std::unique_ptr<abstract_option>> opts;
        std::unordered_map<std::string, abstract_option*> opt_map;
        std::unordered_map<std::string, abstract_option*> alias_map;
        std::vector<std::unique_ptr<option_constraint>> constraints;

    public:
        bool has_value(std::string const &name) const;

        template <typename OptionT>
        OptionT const& get_option(std::string const &name) const
        {
            auto it = opt_map.find(name);
            if(it == opt_map.end()) {
                LOG_FATAL(format("Option %s has not been defined") % name);
            }

            OptionT const *cast_opt = dynamic_cast<OptionT const *>(it->second);
            if(!cast_opt) {
                LOG_FATAL(format("Option %s type mismatch") % name);
            }

            return *cast_opt;
        }

        void insert(std::unique_ptr<abstract_option>&& opt);

        template <typename OptionT, typename... ArgT>
        void emplace(std::string const &name, ArgT&&... args)
        {
            insert(make_unique<OptionT>(name, std::forward<ArgT>(args)...));
        }

        template <typename ConstraintT, typename... ArgT>
        void emplace_constraint(ArgT&&... args)
        {
            constraints.emplace_back(make_unique<ConstraintT>(std::forward<ArgT>(args)...));
        }

        template <typename ArgRangeT>
        void load_from_arg_list(ArgRangeT const &range)
        {
            // Reset all opts.
            for(auto &opt : opts) {
                opt->reset();
            }

            auto arg_list = make_range_argument_queue(range.begin(), range.end());
            while(!arg_list.empty()) {
                auto arg = arg_list.peek();
                arg_list.pop();

                auto it = alias_map.find(arg);
                if(it == alias_map.end()) {
                    LOG_FATAL(format("Unrecognized option %s") % arg);
                }
                else {
                    it->second->load_from_arg_list(arg_list);
                }
            }

            // Evaluate constraints
            for(auto const &constraint : constraints) {
                constraint->check_constraint(*this);
            }
        }

        void add_alias(std::string const &option, std::string const &alias);
    };

}
