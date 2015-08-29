#pragma once

#include "abstract_option.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace gorc {

    template <typename ValueT, typename BindingT, typename DefaultT = ValueT>
    class value_option : public abstract_option {
    private:
        BindingT &binding;
        DefaultT default_value;
        bool is_set = false;

    public:
        value_option(std::string const &name,
                     BindingT &binding,
                     DefaultT const &default_value)
            : abstract_option(name)
            , binding(binding)
            , default_value(default_value)
        {
            return;
        }

        virtual void reset() override
        {
            binding = default_value;
            is_set = false;
        }

        virtual void load_from_arg_list(abstract_argument_queue &arg_list) override
        {
            if(is_set) {
                // Switch is already set, and is being re-set. Raise a warning.
                LOG_WARNING(format("Option %s is already set") % name);
            }

            if(arg_list.empty()) {
                // Option requires a value
                LOG_FATAL(format("Option %s requires a value") % name);
            }

            is_set = true;
            std::string value = arg_list.peek();
            arg_list.pop();

            try {
                binding = boost::lexical_cast<ValueT>(value);
            }
            catch(boost::bad_lexical_cast const &e) {
                LOG_FATAL(format("Could not parse value for option %s: %s") %
                                 name %
                                 e.what());
            }
        }

        virtual bool has_value() const override
        {
            return is_set;
        }
    };

    template <typename ValueT, typename DefaultT = ValueT>
    std::unique_ptr<abstract_option> make_value_option(std::string const &name,
                                                       ValueT &binding,
                                                       DefaultT const &default_value = ValueT())
    {
        return make_unique<value_option<ValueT, ValueT, DefaultT>>(name, binding, default_value);
    }

    template <typename ValueT, typename BindingT, typename DefaultT = BindingT>
    std::unique_ptr<abstract_option> make_value_option(std::string const &name,
                                                       BindingT &binding,
                                                       DefaultT const &default_value = BindingT())
    {
        return make_unique<value_option<ValueT, BindingT, DefaultT>>(name, binding, default_value);
    }

}
