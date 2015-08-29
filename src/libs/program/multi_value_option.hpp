#pragma once

#include "abstract_option.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace gorc {

    template <typename ElementT, typename BindingItT>
    class multi_value_option : public abstract_option {
    private:
        BindingItT binding;
        bool is_set = false;

    public:
        multi_value_option(std::string const &name,
                           BindingItT &binding)
            : abstract_option(name)
            , binding(binding)
        {
            return;
        }

        virtual void reset() override
        {
            is_set = false;
            return;
        }

        virtual void load_from_arg_list(abstract_argument_queue &arg_list) override
        {
            if(arg_list.empty()) {
                LOG_FATAL(format("Option %s requires a value") % name);
            }

            auto em = arg_list.peek();
            arg_list.pop();

            try {
                *binding = boost::lexical_cast<ElementT>(em);
            }
            catch(boost::bad_lexical_cast const &e) {
                LOG_FATAL(format("Could not parse value for option %s: %s") %
                          name %
                          e.what());
            }

            ++binding;
            is_set = true;
        }

        virtual bool has_value() const override
        {
            return is_set;
        }
    };

    template <typename ElementT = std::string, typename OutputItT>
    std::unique_ptr<abstract_option> make_multi_value_option(std::string const &name,
                                                             OutputItT output_iterator)
    {
        return make_unique<multi_value_option<ElementT, OutputItT>>(name, output_iterator);
    }

}
