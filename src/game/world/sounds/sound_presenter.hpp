#pragma once

#include "libold/base/content/assets/sound.hpp"
#include "libold/content/flags/sound_flag.hpp"
#include "libold/content/flags/sound_subclass_type.hpp"
#include "utility/flag_set.hpp"
#include "math/vector.hpp"
#include "libold/base/utility/time.hpp"
#include "game/world/components/thing.hpp"

namespace gorc {
namespace content {
class content_manager;
}

namespace cog {
namespace verbs {
class verb_table;
}
}

namespace game {
class level_state;

namespace world {
class level_model;

namespace sounds {
class sound_model;
class sound;

class sound_presenter {
private:
    content::content_manager& contentmanager;
    level_model* levelModel;
    sound_model* model;

public:
    sound_presenter(content::content_manager&);

    void start(level_model& levelModel, sound_model& soundModel);
    void update(const gorc::time& time);

    void set_ambient_sound(maybe<asset_ref<content::assets::sound>> sound, float volume);
    void play_foley_loop_class(entity_id thing, flags::sound_subclass_type subclass);
    void stop_foley_loop(entity_id thing);

    // sound verbs
    void change_sound_pitch(entity_id channel, float pitch, float delay);
    void change_sound_vol(entity_id channel, float volume, float delay);
    void play_song(int start, int end, int loopto);
    entity_id play_sound_class(entity_id thing, flags::sound_subclass_type subclass);
    entity_id play_sound_local(int wav, float volume, float panning, flag_set<flags::sound_flag> flags);
    entity_id play_sound_pos(int wav, vector<3> pos, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags);
    entity_id play_sound_thing(int wav, entity_id thing, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags);
    void set_music_vol(float volume);
    void stop_sound(entity_id channel, float delay);

    static void register_verbs(cog::verbs::verb_table&, level_state&);
};

}
}
}
}
