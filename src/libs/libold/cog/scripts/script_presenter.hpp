#pragma once

#include "libold/cog/flags/message_type.hpp"
#include "script_model.hpp"
#include "libold/cog/vm/virtual_machine.hpp"
#include "libold/base/utility/time.hpp"
#include "libold/cog/verbs/table.hpp"

namespace gorc {
namespace cog {
class script;
class compiler;

namespace vm {
class virtual_machine;
}

namespace scripts {

class script_model;

class script_presenter {
private:
    script_model* model;
    cog::vm::virtual_machine VirtualMachine;
    cog::verbs::verb_table& verb_table;
    int master_cog = -1;

protected:
    void run_waiting_cogs();

public:
    script_presenter(cog::verbs::verb_table& verb_table);
    virtual ~script_presenter();

    void start(script_model& scriptModel);
    void update(const gorc::time& time);

    void create_global_cog_instance_unlinked(const cog::script& script, cog::compiler& compiler);

    int get_global_cog_instance(cog::script const* script) const;

    cog::vm::value send_message(int InstanceId, cog::message_id message,
            int SenderId, int SenderRef, flags::message_type SenderType,
            int SourceRef = -1, flags::message_type SourceType = flags::message_type::nothing,
            cog::vm::value Param0 = 0, cog::vm::value Param1 = 0, cog::vm::value Param2 = 0, cog::vm::value Param3 = 0);
    void send_message_to_all(cog::message_id message, int SenderId, int SenderRef, flags::message_type SenderType,
            int SourceRef = -1, flags::message_type SourceType = flags::message_type::nothing,
            cog::vm::value Param0 = 0, cog::vm::value Param1 = 0, cog::vm::value Param2 = 0, cog::vm::value Param3 = 0);

    // COG run-time verbs
    inline int get_param(int param_num) { return model->running_cog_state.top().params[param_num % 4]; }
    inline int get_sender_id() { return model->running_cog_state.top().sender_id; }
    inline int get_sender_ref() { return model->running_cog_state.top().sender_ref; }
    inline int get_sender_type() { return static_cast<int>(model->running_cog_state.top().sender_type); }
    inline int get_source_ref() { return model->running_cog_state.top().source_ref; }
    inline int get_source_type() { return static_cast<int>(model->running_cog_state.top().source_type); }

    void set_pulse(float time);
    void set_timer(float time);
    void set_timer_ex(float delay, cog::vm::value id, cog::vm::value param0, cog::vm::value param1);
    void kill_timer_ex(cog::vm::value id);
    void sleep(float time);

    inline int get_master_cog() const {
        return master_cog;
    }

    inline void set_master_cog(int cog) {
        master_cog = cog;
    }

    int get_self_cog() const;

    static void register_verbs(verbs::verb_table&);
};

}
}
}
