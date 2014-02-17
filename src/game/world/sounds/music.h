#pragma once

#include <SFML/Audio.hpp>

namespace gorc {
namespace game {
namespace world {
namespace sounds {

class music {
private:
    sf::Music internal_music;

    bool play = false;

    int start;
    int end;
    int loopto;

    int current;

    static const int num_tracks = 7;

    void internal_play_song(int song);

public:
    void play_song(int start, int end, int loopto);
    void set_volume(float volume);

    void update(double dt);
};

}
}
}
}
