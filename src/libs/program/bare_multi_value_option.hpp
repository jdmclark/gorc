#pragma once

#include "abstract_bare_option.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace gorc {

    template <typename ElementT, typename BindingItT>
    class bare_multi_value_option : public abstract_bare_option {
    private:
        BindingItT binding;
        bool is_set = false;

    public:
        bare_multi_value_option(BindingItT &binding)
            : binding(binding)
        {
            return;
        }

        virtual void reset() override
        {
            is_set = false;
            return;
        }

        virtual void load_from_arg(std::string const &arg) override
        {
            try {
                *binding = boost::lexical_cast<ElementT>(arg);
            }
            catch(boost::bad_lexical_cast const &e) {
                LOG_FATAL(format("Could not parse value for input: %s") %
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
    std::unique_ptr<abstract_bare_option> make_bare_multi_value_option(OutputItT output_iterator)
    {
        return make_unique<bare_multi_value_option<ElementT, OutputItT>>(output_iterator);
    }
}
