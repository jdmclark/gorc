#pragma once

#include "libold/base/content/assets/sound.hpp"
#include "libold/content/flags/sound_flag.hpp"
#include "libold/content/flags/sound_subclass_type.hpp"
#include "utility/flag_set.hpp"
#include "math/vector.hpp"
#include "libold/base/utility/time.hpp"
#include "game/world/components/thing.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "libold/base/utility/entity_id.hpp"

namespace gorc {

class content_manager;

namespace game {
class level_state;

namespace world {
class level_model;

namespace sounds {
class sound_model;
class sound;

class sound_presenter {
private:
    content_manager& contentmanager;
    level_model* levelModel;
    sound_model* model;

public:
    sound_presenter(content_manager&);

    void start(level_model& levelModel, sound_model& soundModel);
    void update(const gorc::time& time);

    void set_ambient_sound(maybe<asset_ref<content::assets::sound>> sound, float volume);
    void play_foley_loop_class(thing_id thing, flags::sound_subclass_type subclass);
    void stop_foley_loop(thing_id thing);

    // sound verbs
    void change_sound_pitch(thing_id channel, float pitch, float delay);
    void change_sound_vol(thing_id channel, float volume, float delay);
    void play_song(int start, int end, int loopto);
    thing_id play_sound_class(thing_id thing, flags::sound_subclass_type subclass);
    thing_id play_sound_local(sound_id wav, float volume, float panning, flag_set<flags::sound_flag> flags);
    thing_id play_sound_pos(sound_id wav, vector<3> pos, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags);
    thing_id play_sound_thing(sound_id wav, thing_id thing, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags);
    void set_music_vol(float volume);
    void stop_sound(thing_id channel, float delay);

    static void register_verbs(cog::verb_table&, level_state&);
};

}
}
}
}
