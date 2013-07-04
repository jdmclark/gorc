#include "thing.h"

Gorc::Game::World::Level::Thing::Thing(const Content::Assets::Template& tpl)
	: Content::Assets::Template(tpl) {
	return;
}

const Gorc::Content::Assets::Template& Gorc::Game::World::Level::Thing::operator=(const Content::Assets::Template& tpl) {
	return Template::operator=(tpl);
}
