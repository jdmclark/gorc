#include "stack.hpp"
#include "utility/range.hpp"
#include "log/log.hpp"
#include <vector>
#include <algorithm>

namespace {

    std::vector<gorc::stack_frame> stack_frames({ gorc::stack_frame() });

    auto get_variable(std::string const &name)
        -> decltype(stack_frames.front().variable_map.find(name))
    {
        for(auto &stack_frame : make_reverse_range(stack_frames)) {
            auto it = stack_frame.variable_map.find(name);
            if(it != stack_frame.variable_map.end()) {
                return it;
            }
        }

        LOG_FATAL(gorc::format("undefined variable '%s'") % name);
    }

}

gorc::scoped_stack_frame::scoped_stack_frame()
{
    stack_frames.push_back(stack_frame());
}

gorc::scoped_stack_frame::~scoped_stack_frame()
{
    stack_frames.pop_back();
}

void gorc::create_variable(std::string const &name, shvalue const &value)
{
    auto res = stack_frames.back().variable_map.emplace(name, value);
    if(!res.second) {
        LOG_FATAL(format("variable '%s' redefinition") % name);
    }
}

gorc::shvalue const& gorc::get_variable_value(std::string const &name)
{
    return get_variable(name)->second;
}

void gorc::set_variable_value(std::string const &name, shvalue const &value)
{
    get_variable(name)->second = value;
}
