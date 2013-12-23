#pragma once

#include "cog/scripts/script_presenter.h"
#include "cog/flags/message_type.h"
#include "script_model.h"
#include "cog/vm/virtual_machine.h"
#include "framework/utility/time.h"

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
class level_state;

namespace world {
class level_model;

namespace scripts {

class script_model;

class script_presenter : public cog::scripts::script_presenter {
private:
	level_model* levelModel;
	script_model* model;
	cog::vm::virtual_machine VirtualMachine;

public:
	script_presenter(level_state& curr_components);

	void start(level_model& levelModel, script_model& scriptModel);

	void create_level_dummy_instances(int count);
	void create_level_cog_instance(int index, const cog::script& script, content::manager& manager, cog::compiler& compiler,
			const std::vector<cog::vm::value>& values);
	void create_global_cog_instance(const cog::script& script, content::manager& manager, cog::compiler& compiler);

	int get_global_cog_instance(cog::script const* script) const;

	void send_message_to_linked(cog::message_id message, int SenderRef, flags::message_type SenderType,
			int SourceRef = -1, flags::message_type SourceType = flags::message_type::nothing,
			cog::vm::value Param0 = 0, cog::vm::value Param1 = 0, cog::vm::value Param2 = 0, cog::vm::value Param3 = 0);

	void resume_wait_for_stop(int wait_thing);

	void wait_for_stop(int thing);
	void capture_thing(int thing);

	int get_thing_class_cog(int thing);

	static void register_verbs(cog::verbs::verb_table&, level_state&);
};

}
}
}
}
