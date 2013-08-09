#include "surface.h"

Gorc::Game::World::Level::Surface::Surface(const Content::Assets::LevelSurface& surf) {
	*this = surf;
}

const Gorc::Content::Assets::LevelSurface& Gorc::Game::World::Level::Surface::operator=(const Content::Assets::LevelSurface& surf) {
	return Content::Assets::LevelSurface::operator=(surf);
}
