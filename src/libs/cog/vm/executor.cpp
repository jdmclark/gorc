#include "executor.hpp"
#include "log/log.hpp"

gorc::cog::executor::executor(verb_table &verbs)
    : verbs(verbs)
{
    services.add(*this);
    services.add(vm);
    return;
}

gorc::cog::executor::executor(deserialization_constructor_tag, binary_input_stream &bis)
    : verbs(bis.services.get<verb_table>())
{
    services.add(*this);
    services.add(vm);

    binary_deserialize_range(bis, std::back_inserter(instances), [](auto &bis) {
            return std::make_unique<instance>(deserialization_constructor, bis);
        });

    binary_deserialize_range(bis, std::back_inserter(sleep_records), [](auto &bis) {
            return std::make_unique<sleep_record>(deserialization_constructor, bis);
        });
}

void gorc::cog::executor::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize_range(bos, instances, [](auto &bos, auto const &em) {
            binary_serialize(bos, *em);
        });

    binary_serialize_range(bos, sleep_records, [](auto &bos, auto const &em) {
            binary_serialize(bos, *em);
        });
}

gorc::cog::instance& gorc::cog::executor::create_instance(asset_ref<cog::script> cog)
{
    instances.push_back(std::make_unique<instance>(cog));
    return *instances.back();
}

gorc::cog::instance& gorc::cog::executor::create_instance(asset_ref<cog::script> cog,
                                                          std::vector<value> const &values)
{
    instances.push_back(std::make_unique<instance>(cog, values));
    return *instances.back();
}

gorc::cog::instance& gorc::cog::executor::get_instance(cog_id instance_id)
{
    return *at_id(instances, instance_id);
}

void gorc::cog::executor::add_sleep_record(std::unique_ptr<sleep_record> &&sr)
{
    sleep_records.push_back(std::forward<std::unique_ptr<sleep_record>>(sr));
}

gorc::maybe<gorc::cog::call_stack_frame> gorc::cog::executor::create_message_frame(
        cog_id target,
        message_type msg,
        value sender,
        value source,
        value param0,
        value param1,
        value param2,
        value param3)
{
    int inst_index = static_cast<int>(target);
    if(inst_index < 0 || static_cast<size_t>(inst_index) >= instances.size()) {
        LOG_ERROR(format("could not send message %s to cog %d: cog id is out of bounds") %
                  as_string(msg) %
                  inst_index);
        return nothing;
    }

    auto &inst = instances.at(static_cast<size_t>(inst_index));
    auto addr = inst->cog->exports.get_offset(msg);
    if(!addr.has_value()) {
        LOG_WARNING(format("sent message %s to cog %d, but the message is not exported") %
                    as_string(msg) %
                    inst_index);
        return nothing;
    }

    return call_stack_frame(target,
                            addr.get_value(),
                            sender,
                            source,
                            param0,
                            param1,
                            param2,
                            param3);
}

void gorc::cog::executor::send_to_all(message_type t,
                                      value sender,
                                      value source,
                                      value param0,
                                      value param1,
                                      value param2,
                                      value param3)
{
    for(size_t i = 0; i < instances.size(); ++i) {
        auto &inst = instances[i];

        auto addr = inst->cog->exports.get_offset(t);
        if(!addr.has_value()) {
            continue;
        }

        continuation cc(call_stack_frame(cog_id(static_cast<int>(i)),
                                         addr.get_value(),
                                         sender,
                                         source,
                                         param0,
                                         param1,
                                         param2,
                                         param3));
        vm.execute(verbs, *this, services, cc);
    }
}

void gorc::cog::executor::update(time_delta dt)
{
    // Decrement times first.
    // Continuations may insert new sleep records later.
    for(auto &sleep_record : sleep_records) {
        sleep_record->expiration_time -= dt;
    }

    for(auto it = sleep_records.begin(); it != sleep_records.end(); ) {
        if((*it)->expiration_time <= 0.0s) {
            auto sr = std::move(*it);
            std::swap(*it, sleep_records.back());
            sleep_records.pop_back();

            vm.execute(verbs, *this, services, sr->cc);
        }
        else {
            ++it;
        }
    }
}
