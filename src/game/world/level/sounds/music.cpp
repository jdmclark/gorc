#include "music.h"
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

void Gorc::Game::World::Level::Sounds::Music::InternalPlaySong(int song) {
	boost::filesystem::path music_file_path = boost::filesystem::path("game/restricted/music/1") /
			boost::str(boost::format("%1%.ogg") % (song + 1));
	music.openFromFile(music_file_path.native());
	music.setLoop(false);
	music.setRelativeToListener(false);
	music.setPosition(0,0,0);
	music.play();
	current = song;
}

void Gorc::Game::World::Level::Sounds::Music::PlaySong(int start, int end, int loopto) {
	// TODO: Currently implemented assuming disc 1. Implement disc switching.
	this->start = (start - 2) % num_tracks;
	this->end = (end - 2) % num_tracks;
	this->loopto = (loopto - 2) % num_tracks;
	play = true;

	music.setVolume(100.0f);
	InternalPlaySong(this->start);
}

void Gorc::Game::World::Level::Sounds::Music::SetVolume(float volume) {
	music.setVolume(100.0f * volume);
}

void Gorc::Game::World::Level::Sounds::Music::Update(double dt) {
	if(play && (music.getStatus() != sf::Music::Playing)) {
		// Advance to next song.
		if(current == end) {
			InternalPlaySong(loopto);
		}
		else {
			current = (current + 1) % num_tracks;
		}
	}
}
