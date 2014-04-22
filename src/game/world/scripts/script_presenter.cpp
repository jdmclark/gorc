#include "script_presenter.h"
#include "script_model.h"
#include "game/level_state.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "base/utility/make_unique.h"

gorc::game::world::scripts::script_presenter::script_presenter(level_state& components)
    : gorc::cog::scripts::script_presenter(components.verb_table), levelModel(nullptr), model(nullptr) {
    return;
}

void gorc::game::world::scripts::script_presenter::start(level_model& levelModel, script_model& scriptModel) {
    this->levelModel = &levelModel;
    model = &scriptModel;

    cog::scripts::script_presenter::start(scriptModel);
}

void gorc::game::world::scripts::script_presenter::resume_wait_for_stop(int wait_thing) {
    // Enqueue waiting cogs
    for(auto& wait_cog : model->wait_for_stop_cogs) {
        if(std::get<0>(wait_cog) == wait_thing) {
            model->running_cog_state.push(std::get<1>(wait_cog));
            model->wait_for_stop_cogs.erase(wait_cog);
        }
    }

    run_waiting_cogs();
}

void gorc::game::world::scripts::script_presenter::create_level_dummy_instances(size_t count) {
    // create empty, non-functional COG instances for padding out the list of level instances.
    for(size_t i = 0; i < count; ++i) {
        model->cogs.emplace_back(std::unique_ptr<cog::instance>(nullptr), cog::scripts::script_timer_state());
    }
}

void gorc::game::world::scripts::script_presenter::create_level_cog_instance(int index, const cog::script& script, content::content_manager& manager,
        cog::compiler&, const std::vector<cog::vm::value>& values) {
    auto& cog_inst_pair = model->cogs[index];
    std::get<0>(cog_inst_pair) = make_unique<cog::instance>(script);
    std::get<1>(cog_inst_pair) = cog::scripts::script_timer_state();

    cog::instance& inst = *std::get<0>(cog_inst_pair);

    inst.heap.resize(script.symbol_table.size());

    auto it = script.symbol_table.begin();
    auto jt = inst.heap.begin();
    auto kt = values.begin();

    for( ; it != script.symbol_table.end() && jt != inst.heap.end(); ++it, ++jt) {
        if(kt != values.end() && !it->local && it->type != cog::symbols::symbol_type::message) {
            (*jt) = *kt;
            ++kt;
        }
        else {
            (*jt) = it->default_value;
        }
    }

    // Load instance resources and set flags
    it = script.symbol_table.begin();
    jt = inst.heap.begin();

    for( ; it != script.symbol_table.end() && jt != inst.heap.end(); ++it, ++jt) {
        switch(it->type) {
        case cog::symbols::symbol_type::material:
            try {
                (*jt) = static_cast<int>(manager.load_id<content::assets::material>(static_cast<const char*>(*jt), *levelModel->level.master_colormap));
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::model:
            try {
                (*jt) = static_cast<int>(manager.load_id<content::assets::model>(static_cast<const char*>(*jt), *levelModel->level.master_colormap));
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::sound:
            try {
                (*jt) = static_cast<int>(manager.load_id<content::assets::sound>(static_cast<const char*>(*jt)));
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::keyframe:
            try {
                (*jt) = static_cast<int>(manager.load_id<content::assets::animation>(static_cast<const char*>(*jt)));
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::thing_template: {
            // Convert template name to lowercase for matching.
            std::string tpl_name(static_cast<const char*>(*jt));
            std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), tolower);
            auto it = levelModel->level.template_map.find(tpl_name);
            if(it == levelModel->level.template_map.end()) {
                // TODO: thing_template not found, report error.
                (*jt) = -1;
            }
            else {
                (*jt) = it->second;
            }
        }
        break;

        case cog::symbols::symbol_type::cog: {
            // Already an integer index.
        }
        break;

        case cog::symbols::symbol_type::sector: {
            int index = static_cast<int>(*jt);
            if(index >= 0) {
                levelModel->sectors[index].flags += flags::sector_flag::CogLinked;
            }
        }
        break;

        case cog::symbols::symbol_type::surface: {
            int index = static_cast<int>(*jt);
            if(index >= 0) {
                levelModel->surfaces[index].flags += flags::surface_flag::CogLinked;
            }
        }
        break;

        case cog::symbols::symbol_type::thing: {
            int index = static_cast<int>(*jt);
            if(index >= 0) {
                levelModel->things[index].flags += flags::thing_flag::CogLinked;
            }
        }
        break;

        case cog::symbols::symbol_type::ai:
            // TODO: Handle AI loading.
        default:
            break;
        }
    }

    // Send startup message
    send_message(index, cog::message_id::startup, -1, -1, flags::message_type::nothing);
}

void gorc::game::world::scripts::script_presenter::create_global_cog_instance(const cog::script& script,
        content::content_manager& manager, cog::compiler&) {
    if(model->global_script_instances.find(&script) != model->global_script_instances.end()) {
        // Instance already created.
        return;
    }

    model->cogs.emplace_back(make_unique<cog::instance>(script), cog::scripts::script_timer_state());
    cog::instance& inst = *std::get<0>(model->cogs.back());
    model->global_script_instances.emplace(&script, model->cogs.size() - 1);

    inst.heap.resize(script.symbol_table.size());

    auto it = script.symbol_table.begin();
    auto jt = inst.heap.begin();

    for( ; it != script.symbol_table.end() && jt != inst.heap.end(); ++it, ++jt) {
        (*jt) = it->default_value;
    }

    // Load instance resources and set flags
    it = script.symbol_table.begin();
    jt = inst.heap.begin();

    for( ; it != script.symbol_table.end() && jt != inst.heap.end(); ++it, ++jt) {
        switch(it->type) {
        case cog::symbols::symbol_type::material:
            try {
                (*jt) = manager.load_id<content::assets::material>(static_cast<const char*>(*jt), *levelModel->level.master_colormap);
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::model:
            try {
                (*jt) = manager.load_id<content::assets::model>(static_cast<const char*>(*jt), *levelModel->level.master_colormap);
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::sound:
            try {
                (*jt) = manager.load_id<content::assets::sound>(static_cast<const char*>(*jt));
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::keyframe:
            try {
                (*jt) = manager.load_id<content::assets::animation>(static_cast<const char*>(*jt));
            }
            catch(...) {
                (*jt) = nullptr;
            }
            break;

        case cog::symbols::symbol_type::thing_template: {
            auto it = levelModel->level.template_map.find(static_cast<const char*>(*jt));
            if(it == levelModel->level.template_map.end()) {
                // TODO: thing_template not found, report error.
                (*jt) = -1;
            }
            else {
                (*jt) = it->second;
            }
        }
        break;

        case cog::symbols::symbol_type::cog: {
            // Already an integer index.
        }
        break;

        case cog::symbols::symbol_type::sector: {
            int index = static_cast<int>(*jt);
            if(index >= 0) {
                levelModel->sectors[index].flags += flags::sector_flag::CogLinked;
            }
        }
        break;

        case cog::symbols::symbol_type::surface: {
            int index = static_cast<int>(*jt);
            if(index >= 0) {
                levelModel->surfaces[index].flags += flags::surface_flag::CogLinked;
            }
        }
        break;

        case cog::symbols::symbol_type::thing: {
            int index = static_cast<int>(*jt);
            if(index >= 0) {
                levelModel->things[index].flags += flags::thing_flag::CogLinked;
            }
        }
        break;

        case cog::symbols::symbol_type::ai:
            // TODO: Handle AI loading.
        default:
            break;
        }
    }

    // Send startup message
    send_message(static_cast<int>(model->cogs.size()) - 1, cog::message_id::startup, -1, -1, flags::message_type::nothing);
}

int gorc::game::world::scripts::script_presenter::get_global_cog_instance(cog::script const* script) const {
    auto it = model->global_script_instances.find(script);
    if(it != model->global_script_instances.end()) {
        return it->second;
    }

    return -1;
}

void gorc::game::world::scripts::script_presenter::send_message_to_linked(cog::message_id message,
        int SenderRef, flags::message_type SenderType,
        int SourceRef, flags::message_type SourceType,
        cog::vm::value Param0, cog::vm::value Param1, cog::vm::value Param2, cog::vm::value Param3) {
    cog::symbols::symbol_type expectedsymbol_type = cog::symbols::symbol_type::integer;

    maybe<size_t> capture_cog;
    maybe<size_t> class_cog;

    int source_mask = 0;
    if(SourceType == flags::message_type::thing) {
        const auto& sender_thing = levelModel->things[SourceRef];
        source_mask = 1 << static_cast<int>(sender_thing.type);
    }

    switch(SenderType) {
    case flags::message_type::sector: {
            expectedsymbol_type = cog::symbols::symbol_type::sector;

            auto& sec = levelModel->sectors[SenderRef];
            if(!(sec.flags & flags::sector_flag::CogLinked)) {
                return;
            }
        }
        break;

    case flags::message_type::surface: {
            expectedsymbol_type = cog::symbols::symbol_type::surface;

            auto& surf = levelModel->surfaces[SenderRef];
            if(!(surf.flags & flags::surface_flag::CogLinked)) {
                return;
            }
        }
        break;

    case flags::message_type::thing: {
            expectedsymbol_type = cog::symbols::symbol_type::thing;

            // Dispatch to capture cog
            thing& thing = levelModel->things[SenderRef];
            if(thing.capture_cog >= 0) {
                capture_cog = thing.capture_cog;
                send_message(thing.capture_cog, message, -1, SenderRef, SenderType,
                        SourceRef, SourceType, Param0, Param1, Param2, Param3);
            }

            // Dispatch to class cog
            if(thing.cog) {
                auto it = model->global_script_instances.find(&thing.cog->cogscript);
                if(it != model->global_script_instances.end()) {
                    class_cog = it->second;
                    send_message(it->second, message, -1, SenderRef, SenderType,
                            SourceRef, SourceType, Param0, Param1, Param2, Param3);
                }
            }

            if(!(thing.flags & flags::thing_flag::CogLinked)) {
                return;
            }
        }
        break;

    case flags::message_type::system:
    case flags::message_type::cog:
    case flags::message_type::nothing:
        // Cannot be coglinked.
        return;
    }

    for(size_t i = 0; i < levelModel->level.cogs.size(); ++i) {
        if(make_maybe(i) == class_cog || make_maybe(i) == capture_cog) {
            continue;
        }

        auto& inst_ptr = std::get<0>(model->cogs[i]);
        if(!inst_ptr) {
            continue;
        }

        cog::instance& inst = *inst_ptr;

        auto it = inst.script.symbol_table.begin();
        auto jt = inst.heap.begin();

        for(; it != inst.script.symbol_table.end() && jt != inst.heap.end(); ++it, ++jt) {
            if(!it->no_link && !it->local && it->type == expectedsymbol_type && static_cast<int>(*jt) == SenderRef
                    && (!source_mask || (it->mask & source_mask))) {
                send_message(static_cast<int>(i), message,
                        it->link_id, SenderRef, SenderType, SourceRef, SourceType,
                        Param0, Param1, Param2, Param3);
                break;
            }
        }
    }
}

void gorc::game::world::scripts::script_presenter::wait_for_stop(int thing) {
    cog::scripts::script_continuation continuation = model->running_cog_state.top();

    continuation.program_counter = VirtualMachine.get_program_counter();

    auto& sleep_tuple = model->wait_for_stop_cogs.emplace();
    std::get<0>(sleep_tuple) = thing;
    std::get<1>(sleep_tuple) = continuation;

    VirtualMachine.abort();
}

void gorc::game::world::scripts::script_presenter::capture_thing(int thing_id) {
    levelModel->things[thing_id].capture_cog = model->running_cog_state.top().instance_id;
}

int gorc::game::world::scripts::script_presenter::get_thing_class_cog(int thing_id) {
    const auto* classcog = levelModel->things[thing_id].cog;
    if(classcog) {
        return get_global_cog_instance(&classcog->cogscript);
    }

    return -1;
}

void gorc::game::world::scripts::script_presenter::register_verbs(cog::verbs::verb_table& verbTable, level_state& components) {

    verbTable.add_verb<void, 2>("sendmessage", [&components](int cog_id, int message) {
        components.current_level_presenter->script_presenter->send_message(cog_id, static_cast<cog::message_id>(message),
                -1, components.current_level_presenter->script_presenter->model->running_cog_state.top().instance_id, flags::message_type::cog,
                static_cast<int>(components.current_level_presenter->get_local_player_thing()), flags::message_type::thing);
    });

    verbTable.add_verb<cog::vm::value, 6>("sendmessageex", [&components](int cog_id, int message,
            cog::vm::value param0, cog::vm::value param1, cog::vm::value param2, cog::vm::value param3) {
        return components.current_level_presenter->script_presenter->send_message(cog_id, static_cast<cog::message_id>(message),
                -1, components.current_level_presenter->script_presenter->model->running_cog_state.top().instance_id, flags::message_type::cog,
                static_cast<int>(components.current_level_presenter->get_local_player_thing()), flags::message_type::thing,
                param0, param1, param2, param3);
    });

    verbTable.add_verb<void, 1>("waitforstop", [&components](int thing_id) {
        components.current_level_presenter->script_presenter->wait_for_stop(thing_id);
    });

    verbTable.add_verb<void, 1>("capturething", [&components](int thing_id) {
        components.current_level_presenter->script_presenter->capture_thing(thing_id);
    });

    verbTable.add_verb<int, 1>("getthingclasscog", [&components](int thing_id) {
        return components.current_level_presenter->script_presenter->get_thing_class_cog(thing_id);
    });
}
