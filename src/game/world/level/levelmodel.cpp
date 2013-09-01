#include "levelmodel.h"
#include "framework/flagset.h"
#include "content/manager.h"
#include "game/constants.h"

Gorc::Game::World::Level::LevelModel::LevelModel(Gorc::Content::Manager& ContentManager, Cog::Compiler& CogCompiler,
		const Gorc::Content::Assets::Level& Level, const Content::Assets::Inventory& inv)
	: Level(Level), Header(Level.Header), Adjoins(Level.Adjoins), Sectors(Level.Sectors), InventoryModel(inv) {

	// Copy surfaces and set up object data.
	std::copy(Level.Surfaces.begin(), Level.Surfaces.end(), std::back_inserter(Surfaces));
	for(auto& sector : Sectors) {
		for(int i = sector.FirstSurface; i < sector.FirstSurface + sector.SurfaceCount; ++i) {
			Surfaces[i].ObjectData.SectorId = sector.Number;
			Surfaces[i].ObjectData.SurfaceId = i;
		}
	}

	return;
}
