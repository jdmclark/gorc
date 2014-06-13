#include "music.h"
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

void gorc::game::world::sounds::music::internal_play_song(int song) {
    boost::filesystem::path music_file_path = boost::filesystem::path("game/resource/music/1") /
            boost::str(boost::format("%1%.ogg") % (song + 1));
#ifdef WIN32
    // HACK: Assume filename doesn't contain non-ASCII characters.
    // This needs to be fixed for proper Windows music file support.
    auto original_filename = music_file_path.native();
    std::string music_filename;
    std::transform(original_filename.begin(),
                   original_filename.end(),
                   std::back_inserter(music_filename),
                   [](wchar_t ch) { return static_cast<char>(ch); });
    internal_music.openFromFile(music_filename);
#else
    internal_music.openFromFile(music_file_path.native());
#endif
    internal_music.setLoop(false);
    internal_music.setRelativeToListener(false);
    internal_music.setPosition(0,0,0);
    internal_music.play();
    current = song;
}

void gorc::game::world::sounds::music::play_song(int start, int end, int loopto) {
    // TODO: Currently implemented assuming disc 1. Implement disc switching.
    this->start = (start - 2) % num_tracks;
    this->end = (end - 2) % num_tracks;
    this->loopto = (loopto - 2) % num_tracks;
    play = true;

    internal_music.setVolume(100.0f);
    internal_play_song(this->start);
}

void gorc::game::world::sounds::music::set_volume(float volume) {
    internal_music.setVolume(100.0f * volume);
}

void gorc::game::world::sounds::music::update(double) {
    if(play && (internal_music.getStatus() != sf::Music::Playing)) {
        // Advance to next song.
        if(current == end) {
            internal_play_song(loopto);
        }
        else {
            current = (current + 1) % num_tracks;
        }
    }
}
