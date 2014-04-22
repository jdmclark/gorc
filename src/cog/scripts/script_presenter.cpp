#include "script_presenter.h"
#include "script_model.h"
#include "game/level_state.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"

gorc::cog::scripts::script_presenter::script_presenter(cog::verbs::verb_table& verb_table)
    : model(nullptr), verb_table(verb_table) {
    return;
}

gorc::cog::scripts::script_presenter::~script_presenter() {
    return;
}

void gorc::cog::scripts::script_presenter::start(script_model& scriptModel) {
    model = &scriptModel;
}

void gorc::cog::scripts::script_presenter::update(const time& time) {
    double dt = time.elapsed_as_seconds();

    // update cogs
    for(unsigned int i = 0; i < model->cogs.size(); ++i) {
        auto& cog = model->cogs[i];
        cog::instance& inst = *std::get<0>(cog);
        script_timer_state& timer_state = std::get<1>(cog);

        if(timer_state.timer_remaining_time > 0.0) {
            timer_state.timer_remaining_time -= dt;
            if(timer_state.timer_remaining_time <= 0.0) {
                timer_state.timer_remaining_time = 0.0;
                send_message(i, cog::message_id::timer, -1, -1, flags::message_type::nothing);
            }
        }

        if(timer_state.pulse_time > 0.0) {
            timer_state.pulse_remaining_time -= dt;
            if(timer_state.pulse_remaining_time <= 0.0) {
                timer_state.pulse_remaining_time = timer_state.pulse_time;
                send_message(i, cog::message_id::pulse, -1, -1, flags::message_type::nothing);
            }
        }
    }

    // Enqueue sleeping cogs
    for(auto& cog : model->sleeping_cogs) {
        std::get<0>(cog) -= dt;
        if(std::get<0>(cog) <= 0.0) {
            model->running_cog_state.push(std::get<1>(cog));
            model->sleeping_cogs.erase(cog);
        }
    }

    // Run sleeping cogs
    run_waiting_cogs();

    // update timers
    for(auto& timer : model->timers) {
        timer.delay -= dt;
        if(timer.delay <= 0.0) {
            send_message(timer.instance_id, cog::message_id::timer, timer.id, 0, flags::message_type::nothing,
                    0, flags::message_type::nothing, timer.param0, timer.param1);
            model->timers.erase(timer.get_id());
        }
    }
}

void gorc::cog::scripts::script_presenter::run_waiting_cogs() {
    while(!model->running_cog_state.empty()) {
        cog::instance& inst = *std::get<0>(model->cogs[model->running_cog_state.top().instance_id]);
        VirtualMachine.execute(inst.heap, inst.script.code, model->running_cog_state.top().program_counter, verb_table, this);
        model->running_cog_state.pop();
    }
}

void gorc::cog::scripts::script_presenter::create_global_cog_instance_unlinked(const cog::script& script, cog::compiler&) {
    if(model->global_script_instances.find(&script) != model->global_script_instances.end()) {
        // Instance already created.
        return;
    }

    model->cogs.emplace_back(std::unique_ptr<cog::instance>(new cog::instance(script)), script_timer_state());
    cog::instance& inst = *std::get<0>(model->cogs.back());
    model->global_script_instances.emplace(&script, model->cogs.size() - 1);

    inst.heap.resize(script.symbol_table.size());

    auto it = script.symbol_table.begin();
    auto jt = inst.heap.begin();

    for( ; it != script.symbol_table.end() && jt != inst.heap.end(); ++it, ++jt) {
        (*jt) = it->default_value;
    }

    // Send startup message
    send_message(static_cast<int>(model->cogs.size()) - 1, cog::message_id::startup, -1, -1, flags::message_type::nothing);
}

int gorc::cog::scripts::script_presenter::get_global_cog_instance(cog::script const* script) const {
    auto it = model->global_script_instances.find(script);
    if(it != model->global_script_instances.end()) {
        return it->second;
    }

    return -1;
}

gorc::cog::vm::value gorc::cog::scripts::script_presenter::send_message(int InstanceId, cog::message_id message,
        int SenderId, int SenderRef, flags::message_type SenderType,
        int SourceRef, flags::message_type SourceType,
        cog::vm::value Param0, cog::vm::value Param1, cog::vm::value Param2, cog::vm::value Param3) {
    if(InstanceId < 0) {
        return -1;
    }

    auto& instance = std::get<0>(model->cogs[InstanceId]);
    if(instance) {
        model->running_cog_state.emplace(InstanceId, SenderId, SenderRef, SenderType, SourceRef, SourceType, Param0, Param1, Param2, Param3);

        instance->call(verb_table, VirtualMachine, message, this);

        cog::vm::value rex_val = model->running_cog_state.top().returnex_value;

        model->running_cog_state.pop();

        return rex_val;
    }

    return -1;
}

void gorc::cog::scripts::script_presenter::send_message_to_all(cog::message_id message,
        int SenderId, int SenderRef, flags::message_type SenderType,
        int SourceRef, flags::message_type SourceType,
        cog::vm::value Param0, cog::vm::value Param1, cog::vm::value Param2, cog::vm::value Param3) {
    for(unsigned int i = 0; i < model->cogs.size(); ++i) {
        send_message(i, message, SenderId, SenderRef, SenderType, SourceRef, SourceType, Param0, Param1, Param2, Param3);
    }
}

void gorc::cog::scripts::script_presenter::set_pulse(float time) {
    script_timer_state& state = std::get<1>(model->cogs[model->running_cog_state.top().instance_id]);
    state.pulse_time = time;
    state.pulse_remaining_time = time;
}

void gorc::cog::scripts::script_presenter::set_timer(float time) {
    std::get<1>(model->cogs[model->running_cog_state.top().instance_id]).timer_remaining_time = time;
}

void gorc::cog::scripts::script_presenter::set_timer_ex(float time, cog::vm::value id, cog::vm::value param0, cog::vm::value param1) {
    script_timer& timer = model->timers.emplace();
    timer.instance_id = model->running_cog_state.top().instance_id;
    timer.delay = time;
    timer.id = id;
    timer.param0 = param0;
    timer.param1 = param1;
}

void gorc::cog::scripts::script_presenter::kill_timer_ex(cog::vm::value id) {
    for(auto& timer : model->timers) {
        if(timer.instance_id == model->running_cog_state.top().instance_id && timer.id == id) {
            model->timers.erase(timer);
            return;
        }
    }
}

void gorc::cog::scripts::script_presenter::sleep(float time) {
    script_continuation continuation = model->running_cog_state.top();

    continuation.program_counter = VirtualMachine.get_program_counter();

    auto& sleep_tuple = model->sleeping_cogs.emplace();
    std::get<0>(sleep_tuple) = time;
    std::get<1>(sleep_tuple) = continuation;

    VirtualMachine.abort();
}

int gorc::cog::scripts::script_presenter::get_self_cog() const {
    return model->running_cog_state.top().instance_id;
}

void gorc::cog::scripts::script_presenter::register_verbs(cog::verbs::verb_table& verbTable) {
    verbTable.add_system_verb<int, 1, script_presenter>("getparam", [](int param_num, script_presenter& sp) {
        return sp.get_param(param_num);
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getsenderid", [](script_presenter& sp) {
        return sp.get_sender_id();
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getsenderref", [](script_presenter& sp) {
        return sp.get_sender_ref();
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getsendertype", [](script_presenter& sp) {
        return sp.get_sender_type();
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getsourceref", [](script_presenter& sp) {
        return sp.get_source_ref();
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getsourcetype", [](script_presenter& sp) {
        return sp.get_source_type();
    });

    verbTable.add_system_verb<void, 1, script_presenter>("returnex", [](cog::vm::value value, script_presenter& sp) {
        sp.model->running_cog_state.top().returnex_value = value;
    });

    verbTable.add_system_verb<void, 1, script_presenter>("setpulse", [](float time, script_presenter& sp) {
        sp.set_pulse(time);
    });

    verbTable.add_system_verb<void, 1, script_presenter>("settimer", [](float time, script_presenter& sp) {
        sp.set_timer(time);
    });

    verbTable.add_system_verb<void, 4, script_presenter>("settimerex", [](float time, int id, cog::vm::value param0, cog::vm::value param1, script_presenter& sp) {
        sp.set_timer_ex(time, id, param0, param1);
    });

    verbTable.add_system_verb<void, 1, script_presenter>("killtimerex", [](int id, script_presenter& sp) {
        sp.kill_timer_ex(id);
    });

    verbTable.add_system_verb<void, 1, script_presenter>("sleep", [](float time, script_presenter& sp) {
        sp.sleep(time);
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getmastercog", [](script_presenter& sp) {
        return sp.get_master_cog();
    });

    verbTable.add_system_verb<int, 0, script_presenter>("getselfcog", [](script_presenter& sp) {
        return sp.get_self_cog();
    });

    verbTable.add_system_verb<void, 1, script_presenter>("setmastercog", [](int cog, script_presenter& sp) {
        sp.set_master_cog(cog);
    });
}
