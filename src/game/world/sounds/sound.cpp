#include "framework/content/assets/sound.h"
#include "game/world/level_model.h"
#include "framework/math/util.h"

void gorc::game::world::sounds::sound::play_ambient(const content::assets::sound& buffer, float volume, float panning, flag_set<flags::sound_flag> flags) {
    expired = false;
    update_position = false;

    internal_sound.setBuffer(buffer.buffer);
    internal_sound.setPosition(panning, 0.0f, 0.0f);
    internal_sound.setRelativeToListener(true);
    internal_sound.setVolume(volume * 100.0f);
    internal_sound.setLoop(flags & flags::sound_flag::Loops);
    internal_sound.setPitch(1.0f);
    internal_sound.setAttenuation(0.0f);
    internal_sound.setMinDistance(1.0f);
    internal_sound.play();

    do_distance_attenuation = false;

    vol.set(volume, 0.0f);
    pitch.set(1.0f, 0.0f);
}

void gorc::game::world::sounds::sound::play_voice(const content::assets::sound& buffer, float volume, flag_set<flags::sound_flag> flags) {
    expired = false;
    update_position = false;

    internal_sound.setBuffer(buffer.buffer);
    internal_sound.setPosition(0.0f, 0.0f, 1.0f);
    internal_sound.setRelativeToListener(true);
    internal_sound.setVolume(volume * 100.0f);
    internal_sound.setLoop(flags & flags::sound_flag::Loops);
    internal_sound.setPitch(1.0f);
    internal_sound.setAttenuation(0.0f);
    internal_sound.setMinDistance(1.0f);
    internal_sound.play();

    do_distance_attenuation = false;

    vol.set(volume, 0.0f);
    pitch.set(1.0f, 0.0f);
}

void gorc::game::world::sounds::sound::play_positional(const content::assets::sound& buffer, const vector<3>& position,
        float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
    expired = false;

    // TODO: Handle ambient flag.

    actual_min_rad = std::min(minrad, maxrad);
    actual_max_rad = std::max(minrad, maxrad);

    if(actual_min_rad < 0.0f) {
        // TODO: Find out what the actual default min distance is.
        actual_min_rad = 0.2f;
    }

    if(actual_max_rad < 0.0f || (flags & flags::sound_flag::VolumeQuickFalloff)) {
        // TODO: Find out what the actual default max distance is.
        actual_max_rad = actual_min_rad + 0.5f;
    }

    internal_sound.setBuffer(buffer.buffer);
    internal_sound.setPosition(get<0>(position), get<2>(position), -get<1>(position));
    internal_sound.setRelativeToListener(false);
    internal_sound.setVolume(volume * 100.0f);
    internal_sound.setLoop(flags & flags::sound_flag::Loops);
    internal_sound.setMinDistance(actual_min_rad);
    internal_sound.setAttenuation(0.0f);
    internal_sound.setPitch(1.0f);
    internal_sound.play();

    vol.set(volume, 0.0f);
    pitch.set(1.0f, 0.0f);

    if(flags & flags::sound_flag::MinimalVolume) {
        vol.set(0.1f, 0.0f);
    }

    do_distance_attenuation = true;
    this->position = position;
}

void gorc::game::world::sounds::sound::play_sound_local(const level_model& model, const content::assets::sound& sound, float volume,
        float panning, flag_set<flags::sound_flag> flags) {
    if(flags & flags::sound_flag::Voice) {
        play_voice(sound, volume, flags);
    }
    else {
        play_ambient(sound, volume, panning, flags);
    }

    update(0.0, model);
}

void gorc::game::world::sounds::sound::play_sound_pos(const level_model& model, const content::assets::sound& sound,
        const vector<3>& pos, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
    update_position = false;
    play_positional(sound, pos, volume, minrad, maxrad, flags);

    update(0.0, model);
}

void gorc::game::world::sounds::sound::play_sound_thing(const level_model& model, const content::assets::sound& sound,
        int thing, float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
    this->thing = thing;
    update_position = (flags & flags::sound_flag::ThingOriginMovesWithThing);

    vector<3> pos = model.things[thing].position;
    play_positional(sound, pos, volume, minrad, maxrad, flags);

    update(0.0, model);
}

void gorc::game::world::sounds::sound::set_pitch(float pv, float delay) {
    pitch.set(pv, delay);
}

void gorc::game::world::sounds::sound::set_volume(float volume, float delay) {
    vol.set(volume, delay);
}

void gorc::game::world::sounds::sound::stop() {
    internal_sound.stop();
}

void gorc::game::world::sounds::sound::stop(float delay) {
    if(delay <= 0.0f) {
        internal_sound.stop();
    }
    else {
        stop_delay = delay;
        set_volume(0.0f, delay);
    }
}

void gorc::game::world::sounds::sound::update(double dt, const level_model& model) {
    if(stop_delay > 0.0f) {
        stop_delay -= dt;

        if(stop_delay <= 0.0f) {
            internal_sound.stop();
        }
    }

    if(internal_sound.getStatus() == sf::Sound::Stopped) {
        expired = true;
        return;
    }

    vol.update(dt);
    pitch.update(dt);

    if(update_position) {
        vector<3> pos = model.things[thing].position;
        position = pos;
        internal_sound.setPosition(get<0>(pos), get<2>(pos), -get<1>(pos));
    }

    if(do_distance_attenuation) {
        auto sound_dist = length(model.camera_model.current_computed_state.position - position);

        float attenuation = (actual_min_rad != actual_max_rad) ? ((sound_dist - actual_max_rad) / (actual_min_rad - actual_max_rad)) : 1.0f;
        attenuation = clamp(attenuation, 0.0f, 1.0f);

        //std::cout << "DIST: " << sound_dist << " MIN: " << actual_min_rad << " MAX: " << actual_max_rad << " ATTEN:" << attenuation << std::endl;

        internal_sound.setVolume(attenuation * vol * 100.0f);
        internal_sound.setPitch(pitch);
    }
    else {
        internal_sound.setVolume(vol * 100.0f);
        internal_sound.setPitch(pitch);
    }
}

gorc::game::world::sounds::sound::~sound() {
    internal_sound.stop();
}
