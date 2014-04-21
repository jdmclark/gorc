#pragma once

#include "framework/content/assets/sound.h"
#include "content/flags/sound_flag.h"
#include "content/flags/sound_subclass_type.h"
#include "framework/utility/flag_set.h"
#include "framework/math/vector.h"
#include "framework/utility/time.h"

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
class thing;

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
    void update(const time& time);

    void expunge_thing_sounds(int thing_id);

    void set_ambient_sound(content::assets::sound const* sound, float volume);
    void play_foley_loop_class(int thing, flags::sound_subclass_type subclass);
    void stop_foley_loop(int thing);

    // sound verbs
    void change_sound_pitch(int channel, float pitch, float delay);
    void change_sound_vol(int channel, float volume, float delay);
    void play_song(int start, int end, int loopto);
    int play_sound_class(int thing, flags::sound_subclass_type subclass);
    int play_sound_local(int wav, float volume, float panning, flag_set<flags::sound_flag> flags);
    int play_sound_pos(int wav, vector<3> pos, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags);
    int play_sound_thing(int wav, int thing, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags);
    void set_music_vol(float volume);
    void stop_sound(int channel, float delay);

    static void register_verbs(cog::verbs::verb_table&, level_state&);
};

}
}
}
}
