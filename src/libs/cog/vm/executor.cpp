#include "executor.hpp"
#include "log/log.hpp"
#include "utility/range.hpp"

bool gorc::cog::detail::executor_link_comp::operator()(value left, value right) const
{
    return (left.get_type() < right.get_type()) && (left < right);
}

bool gorc::cog::detail::executor_gi_comp::operator()(
        asset_ref<script> left,
        asset_ref<script> right) const
{
    using UT = std::underlying_type<asset_id>::type;
    return static_cast<UT>(left.get_id()) < static_cast<UT>(right.get_id());
}

gorc::cog::executor_linkage::executor_linkage(flag_set<source_type> mask,
                                              cog_id instance_id,
                                              value sender_link_id)
    : mask(mask)
    , instance_id(instance_id)
    , sender_link_id(sender_link_id)
{
    return;
}

gorc::cog::executor_linkage::executor_linkage(deserialization_constructor_tag,
                                              binary_input_stream &is)
    : mask(binary_deserialize<flag_set<source_type>>(is))
    , instance_id(binary_deserialize<cog_id>(is))
    , sender_link_id(binary_deserialize<value>(is))
{
    return;
}

void gorc::cog::executor_linkage::binary_serialize_object(binary_output_stream &os) const
{
    binary_serialize(os, mask);
    binary_serialize(os, instance_id);
    binary_serialize(os, sender_link_id);
}

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

    binary_deserialize_range(bis, std::inserter(linkages, linkages.end()), [](auto &bis) {
            auto obj = binary_deserialize<value>(bis);
            auto st = binary_deserialize<executor_linkage>(bis);
            return std::make_pair(obj, st);
        });

    binary_deserialize_range(bis, std::inserter(global_instance_map, global_instance_map.end()),
        [](auto &bis) {
            auto cog_ref = binary_deserialize<asset_ref<script>>(bis);
            auto inst = binary_deserialize<cog_id>(bis);
            return std::make_pair(cog_ref, inst);
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

    binary_serialize_range(bos, linkages, [](auto &bos, auto const &em) {
            binary_serialize(bos, em.first);
            binary_serialize(bos, em.second);
        });

    binary_serialize_range(bos, global_instance_map, [](auto &bos, auto const &em) {
            binary_serialize(bos, em.first);
            binary_serialize(bos, em.second);
        });
}

void gorc::cog::executor::add_linkage(cog_id id, instance const &inst)
{
    for(auto const &link : inst.linkages) {
        linkages.emplace(link.object,
                         executor_linkage(link.mask,
                                          id,
                                          link.sender_link_id));
    }
}

gorc::cog::instance& gorc::cog::executor::create_instance(asset_ref<cog::script> cog)
{
    cog_id new_cog(static_cast<int>(instances.size()));
    instances.push_back(std::make_unique<instance>(cog));
    add_linkage(new_cog, *instances.back());
    return *instances.back();
}

gorc::cog::instance& gorc::cog::executor::create_instance(asset_ref<cog::script> cog,
                                                          std::vector<value> const &values)
{
    cog_id new_cog(static_cast<int>(instances.size()));
    instances.push_back(std::make_unique<instance>(cog, values));
    add_linkage(new_cog, *instances.back());
    return *instances.back();
}

gorc::cog::instance& gorc::cog::executor::create_global_instance(asset_ref<cog::script> cog)
{
    auto it = global_instance_map.find(cog);
    if(it != global_instance_map.end()) {
        return *at_id(instances, it->second);
    }

    cog_id new_cog(static_cast<int>(instances.size()));
    instances.push_back(std::make_unique<instance>(cog));
    add_linkage(new_cog, *instances.back());
    global_instance_map.emplace(cog, new_cog);
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
        value sender_id,
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
                            sender_id,
                            source,
                            param0,
                            param1,
                            param2,
                            param3);
}

void gorc::cog::executor::send_to_all(message_type t,
                                      value sender,
                                      value sender_id,
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
                                         sender_id,
                                         source,
                                         param0,
                                         param1,
                                         param2,
                                         param3));
        vm.execute(verbs, *this, services, cc);
    }
}

void gorc::cog::executor::send_to_linked(message_type t,
                                         value sender,
                                         value source,
                                         source_type st,
                                         value param0,
                                         value param1,
                                         value param2,
                                         value param3)
{
    for(auto const &link : make_range(linkages.equal_range(sender))) {
        if(!(link.second.mask & st)) {
            // This source type is masked. Don't dispatch message.
            continue;
        }

        auto &inst = at_id(instances, link.second.instance_id);

        auto addr = inst->cog->exports.get_offset(t);
        if(!addr.has_value()) {
            continue;
        }

        continuation cc(call_stack_frame(link.second.instance_id,
                                         addr.get_value(),
                                         sender,
                                         link.second.sender_link_id,
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
