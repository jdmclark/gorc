#include "executor.hpp"
#include "log/log.hpp"
#include "utility/range.hpp"

bool gorc::cog::detail::executor_link_comp::operator()(value left, value right) const
{
    return std::make_tuple(left.get_type(), left) < std::make_tuple(right.get_type(), right);
}

bool gorc::cog::detail::executor_wait_comp::operator()(
        std::tuple<message_type, value> const &left,
        std::tuple<message_type, value> const &right) const
{
    return std::make_tuple(std::get<0>(left), std::get<1>(left).get_type(), std::get<1>(left)) <
           std::make_tuple(std::get<0>(right), std::get<1>(right).get_type(), std::get<1>(right));
}

bool gorc::cog::detail::executor_gi_comp::operator()(
        asset_ref<script> left,
        asset_ref<script> right) const
{
    using UT = std::underlying_type<asset_id>::type;
    return static_cast<UT>(left.get_id()) < static_cast<UT>(right.get_id());
}

bool gorc::cog::detail::executor_timer_comp::operator()(
        std::tuple<cog_id, value> const &left,
        std::tuple<cog_id, value> const &right) const
{
    return std::make_tuple(std::get<0>(left), std::get<1>(left).get_type(), std::get<1>(left)) <
           std::make_tuple(std::get<0>(right), std::get<1>(right).get_type(), std::get<1>(right));
}

gorc::cog::executor::executor(service_registry const &parent)
    : verbs(parent.get<verb_table>())
    , services(&parent)
{
    services.add(*this);
    services.add(vm);
    return;
}

gorc::cog::executor::executor(deserialization_constructor_tag, binary_input_stream &bis)
    : verbs(bis.services.get<verb_table>())
    , services(&bis.services)
{
    services.add(*this);
    services.add(vm);

    binary_deserialize_range(bis, std::back_inserter(instances), [](auto &bis) {
            return std::make_unique<instance>(deserialization_constructor, bis);
        });

    binary_deserialize_range(bis, std::back_inserter(sleep_records), [](auto &bis) {
            return std::make_unique<sleep_record>(deserialization_constructor, bis);
        });

    binary_deserialize_range(bis, std::inserter(wait_records, wait_records.end()), [](auto &bis) {
            auto mt = binary_deserialize<message_type>(bis);
            auto obj = binary_deserialize<value>(bis);
            auto cont = std::make_unique<continuation>(deserialization_constructor, bis);
            return std::make_pair(std::make_tuple(mt, obj), std::move(cont));
        });

    binary_deserialize_range(bis, std::inserter(pulse_records, pulse_records.end()), [](auto &is) {
            auto inst = binary_deserialize<cog_id>(is);
            return std::make_pair(inst, pulse_record(deserialization_constructor, is));
        });

    binary_deserialize_range(bis, std::inserter(timer_records, timer_records.end()), [](auto &is) {
            auto inst = binary_deserialize<cog_id>(is);
            auto timer_id = binary_deserialize<value>(is);
            return std::make_pair(std::make_tuple(inst, timer_id),
                                  timer_record(deserialization_constructor, is));
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

    master_cog = binary_deserialize<cog_id>(bis);
}

void gorc::cog::executor::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize_range(bos, instances, [](auto &bos, auto const &em) {
            binary_serialize(bos, *em);
        });

    binary_serialize_range(bos, sleep_records, [](auto &bos, auto const &em) {
            binary_serialize(bos, *em);
        });

    binary_serialize_range(bos, wait_records, [](auto &bos, auto const &em) {
            binary_serialize(bos, std::get<0>(em.first));
            binary_serialize(bos, std::get<1>(em.first));
            binary_serialize(bos, *em.second);
        });

    binary_serialize_range(bos, pulse_records, [](auto &bos, auto const &em) {
            binary_serialize(bos, em.first);
            binary_serialize(bos, em.second);
        });

    binary_serialize_range(bos, timer_records, [](auto &bos, auto const &em) {
            binary_serialize(bos, std::get<0>(em.first));
            binary_serialize(bos, std::get<1>(em.first));
            binary_serialize(bos, em.second);
        });

    binary_serialize_range(bos, linkages, [](auto &bos, auto const &em) {
            binary_serialize(bos, em.first);
            binary_serialize(bos, em.second);
        });

    binary_serialize_range(bos, global_instance_map, [](auto &bos, auto const &em) {
            binary_serialize(bos, em.first);
            binary_serialize(bos, em.second);
        });

    binary_serialize(bos, master_cog);
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

gorc::cog_id gorc::cog::executor::create_instance(asset_ref<cog::script> cog)
{
    /* Creating this instance may create more instances. Reserve space. */
    cog_id new_cog(static_cast<int>(instances.size()));
    instances.push_back(nullptr);
    at_id(instances, new_cog) = std::make_unique<instance>(services, cog);
    add_linkage(new_cog, *instances.back());
    return new_cog;
}

gorc::cog_id gorc::cog::executor::create_instance(asset_ref<cog::script> cog,
                                                  std::vector<value> const &values)
{
    /* Creating this instance may create more instances. Reserve space. */
    cog_id new_cog(static_cast<int>(instances.size()));
    instances.push_back(nullptr);
    at_id(instances, new_cog) = std::make_unique<instance>(services, cog, values);
    add_linkage(new_cog, *instances.back());
    return new_cog;
}

gorc::cog_id gorc::cog::executor::create_global_instance(asset_ref<cog::script> cog)
{
    auto it = global_instance_map.find(cog);
    if(it != global_instance_map.end()) {
        return it->second;
    }

    /* Creating this instance may create more instances. Reserve space. */
    cog_id new_cog(static_cast<int>(instances.size()));
    instances.push_back(nullptr);
    global_instance_map.emplace(cog, new_cog);

    at_id(instances, new_cog) = std::make_unique<instance>(services, cog);
    add_linkage(new_cog, *instances.back());
    return new_cog;
}

gorc::cog::instance& gorc::cog::executor::get_instance(cog_id instance_id)
{
    return *at_id(instances, instance_id);
}

void gorc::cog::executor::add_sleep_record(std::unique_ptr<sleep_record> &&sr)
{
    sleep_records.push_back(std::forward<std::unique_ptr<sleep_record>>(sr));
}

void gorc::cog::executor::add_wait_record(message_type msg,
                                          value sender,
                                          std::unique_ptr<continuation> &&cc)
{
    wait_records.emplace(std::make_tuple(msg, sender),
                         std::forward<std::unique_ptr<continuation>>(cc));
}

void gorc::cog::executor::add_timer_record(cog_id instance_id,
                                           value timer_id,
                                           time_delta duration,
                                           value param0,
                                           value param1)
{
    timer_records.emplace(std::make_tuple(instance_id, timer_id),
                          timer_record(duration, param0, param1));
}

void gorc::cog::executor::erase_timer_record(cog_id instance_id,
                                             value timer_id)
{
    timer_records.erase(std::make_tuple(instance_id, timer_id));
}

void gorc::cog::executor::set_pulse(cog_id instance_id,
                                    maybe<time_delta> duration)
{
    pulse_records.erase(instance_id);
    maybe_if(duration, [&](time_delta dt) {
            pulse_records.emplace(instance_id, pulse_record(dt));
        });
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

gorc::cog::value gorc::cog::executor::send(cog_id instance,
                                           message_type t,
                                           value sender,
                                           value sender_id,
                                           value source,
                                           value param0,
                                           value param1,
                                           value param2,
                                           value param3)
{
    auto &inst = at_id(instances, instance);

    auto addr = inst->cog->exports.get_offset(t);
    if(!addr.has_value()) {
        return value();
    }

    continuation cc(call_stack_frame(instance,
                                     addr.get_value(),
                                     sender,
                                     sender_id,
                                     source,
                                     param0,
                                     param1,
                                     param2,
                                     param3));
    return vm.execute(verbs, *this, services, cc);
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
        // System source type cannot be masked.
        if(!(link.second.mask & st) && (st != source_type::system)) {
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

    auto wait_rng = wait_records.equal_range(std::make_tuple(t, sender));
    for(auto it = wait_rng.first; it != wait_rng.second;) {
        auto curr_it = it++;
        vm.execute(verbs, *this, services, *curr_it->second);
        wait_records.erase(curr_it);
    }
}

void gorc::cog::executor::set_master_cog(cog_id id)
{
    master_cog = id;
}

gorc::cog_id gorc::cog::executor::get_master_cog() const
{
    return master_cog;
}

void gorc::cog::executor::update(time_delta dt)
{
    // Decrement times first.
    // Continuations may insert new records. The new records shouldn't be decremented this frame.
    for(auto &timer_record : timer_records) {
        timer_record.second.remaining -= dt;
    }

    for(auto &pulse_record : pulse_records) {
        pulse_record.second.remaining -= dt;
    }

    for(auto &sleep_record : sleep_records) {
        sleep_record->expiration_time -= dt;
    }

    // Intentionally quadratic. The timer records data structure may be modified by timer event
    // handlers. Repeatedly searching the map is still more efficient than copying this data.
    while(true) {
        bool seen_any = false;
        for(auto it = timer_records.begin(); it != timer_records.end(); ++it) {
            if(it->second.remaining <= 0.0s) {
                cog_id instance = std::get<0>(it->first);
                value sender_id = std::get<1>(it->first);
                value param0 = it->second.param0;
                value param1 = it->second.param1;

                timer_records.erase(it);

                send(instance,
                     message_type::timer,
                     /* sender */ value(),
                     sender_id,
                     /* source */ value(),
                     param0,
                     param1);

                seen_any = true;
                break;
            }
        }

        if(!seen_any) {
            break;
        }
    }

    // Intentionally quadratic. The pulse records data structure may be modified by pulse event
    // handlers. Repeatedly searching the map is still more efficient than copying this data.
    while(true) {
        bool seen_any = false;
        for(auto it = pulse_records.begin(); it != pulse_records.end(); ++it) {
            if(it->second.remaining <= 0.0s) {
                it->second.remaining += it->second.duration;

                send(it->first,
                     message_type::pulse,
                     /* sender */ value(),
                     /* sender id */ value(),
                     /* source */ value());

                seen_any = true;
                break;
            }
        }

        if(!seen_any) {
            break;
        }
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
