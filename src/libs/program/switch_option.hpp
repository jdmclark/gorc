#pragma once

#include "abstract_option.hpp"
#include <memory>

namespace gorc {

    class switch_option : public abstract_option {
    private:
        bool &binding;

    public:
        switch_option(std::string const &name, bool &binding);

        virtual void reset() override;
        virtual void load_from_arg_list(abstract_argument_queue &arg_list) override;

        virtual bool has_value() const override;
    };

    std::unique_ptr<abstract_option> make_switch_option(std::string const &name, bool &binding);

}
