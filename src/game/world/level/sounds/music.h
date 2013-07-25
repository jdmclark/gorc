#pragma once

#include <SFML/Audio.hpp>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Sounds {

class Music {
private:
	sf::Music music;

	bool play = false;

	int start;
	int end;
	int loopto;

	int current;

	static const int num_tracks = 7;

	void InternalPlaySong(int song);

public:
	void PlaySong(int start, int end, int loopto);
	void SetVolume(float volume);

	void Update(double dt);
};

}
}
}
}
}
