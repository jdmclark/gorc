#pragma once

#include "game/flags/message_type.h"
#include "script_model.h"
#include "cog/vm/virtual_machine.h"

namespace gorc {
namespace content {
class manager;
}

namespace cog {
class script;
class compiler;

namespace verbs {
class verb_table;
}

namespace vm {
class virtual_machine;
}
}

namespace game {
class components;

namespace world {
namespace level {
class level_model;

namespace scripts {

class script_model;

class script_presenter {
private:
	components& components;
	level_model* levelModel;
	script_model* model;
	cog::vm::virtual_machine VirtualMachine;
	int master_cog = -1;

public:
	script_presenter(class components& components);

	void start(level_model& levelModel, script_model& scriptModel);
	void update(double dt);

	void run_waiting_cogs();

	void create_level_dummy_instances(int count);
	void create_level_cog_instance(int index, const cog::script& script, content::manager& manager, cog::compiler& compiler,
			const std::vector<cog::vm::value>& values);
	void create_global_cog_instance(const cog::script& script, content::manager& manager, cog::compiler& compiler);

	int get_global_cog_instance(cog::script const* script) const;

	cog::vm::value send_message(int InstanceId, cog::message_id message,
			int SenderId, int SenderRef, flags::message_type SenderType,
			int SourceRef = -1, flags::message_type SourceType = flags::message_type::nothing,
			cog::vm::value Param0 = 0, cog::vm::value Param1 = 0, cog::vm::value Param2 = 0, cog::vm::value Param3 = 0);
	void send_message_to_all(cog::message_id message, int SenderId, int SenderRef, flags::message_type SenderType,
			int SourceRef = -1, flags::message_type SourceType = flags::message_type::nothing,
			cog::vm::value Param0 = 0, cog::vm::value Param1 = 0, cog::vm::value Param2 = 0, cog::vm::value Param3 = 0);
	void send_message_to_linked(cog::message_id message, int SenderRef, flags::message_type SenderType,
			int SourceRef = -1, flags::message_type SourceType = flags::message_type::nothing,
			cog::vm::value Param0 = 0, cog::vm::value Param1 = 0, cog::vm::value Param2 = 0, cog::vm::value Param3 = 0);

	void resume_wait_for_stop(int wait_thing);

	// COG run-time verbs
	inline int get_param(int param_num) { return model->running_cog_state.top().params[param_num % 4]; }
	inline int get_sender_id() { return model->running_cog_state.top().sender_id; }
	inline int get_sender_ref() { return model->running_cog_state.top().sender_ref; }
	inline int get_sender_type() { return static_cast<int>(model->running_cog_state.top().sender_type); }
	inline int get_source_ref() { return model->running_cog_state.top().source_ref; }
	inline int get_source_type() { return static_cast<int>(model->running_cog_state.top().source_type); }

	void set_pulse(float time);
	void set_timer(float time);
	void set_timer_ex(float delay, int id, cog::vm::value param0, cog::vm::value param1);
	void sleep(float time);
	void wait_for_stop(int thing);
	void capture_thing(int thing);

	inline int get_master_cog() const {
		return master_cog;
	}

	inline void set_master_cog(int cog) {
		master_cog = cog;
	}

	int get_self_cog() const;

	static void register_verbs(cog::verbs::verb_table&, class components&);
};

}
}
}
}
}
