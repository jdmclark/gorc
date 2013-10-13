#include "level_place.h"

Gorc::Game::World::Level::LevelPlace::LevelPlace(std::shared_ptr<Content::Manager> ContentManager,
		const Content::Assets::Level& Level)
	: ContentManager(ContentManager), Level(Level) {
	return;
}

void Gorc::Game::World::Level::LevelPlace::Accept(PlaceVisitor& v) const {
	v.VisitLevelPlace(*this);
}
