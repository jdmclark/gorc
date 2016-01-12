#include "executor.hpp"

gorc::cog::executor::executor(verb_table &verbs)
    : verbs(verbs)
{
    services.add(*this);
    services.add(vm);
    return;
}

gorc::cog::instance& gorc::cog::executor::create_instance(cog::script const &cog)
{
    instances.push_back(std::make_unique<instance>(cog));
    return *instances.back();
}

gorc::cog::instance& gorc::cog::executor::create_instance(cog::script const &cog,
                                                          std::vector<value> const &values)
{
    instances.push_back(std::make_unique<instance>(cog, values));
    return *instances.back();
}

void gorc::cog::executor::send_to_all(message_type t)
{
    for(auto &inst : instances) {
        auto addr = inst->cog.exports.get_offset(t);
        if(!addr.has_value()) {
            continue;
        }

        continuation cc(call_stack_frame(inst->cog, inst->memory, addr.get_value()));

        vm.execute(verbs, services, cc);
    }
}
