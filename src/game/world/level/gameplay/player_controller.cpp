#include "player_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/world/level/physics/query.h"

void Gorc::Game::World::Level::Gameplay::PlayerController::FindSectorRestingManifolds(const Physics::Sphere& sphere, int sector_id,
		std::set<int>& closed, std::vector<Math::Vector<3>>& manifolds) {
	if(closed.find(sector_id) != closed.end()) {
		return;
	}

	closed.insert(sector_id);

	const auto& sector = presenter.Model->Sectors[sector_id];
	for(int i = sector.FirstSurface; i < sector.FirstSurface + sector.SurfaceCount; ++i) {
		const auto& surface = presenter.Model->Surfaces[i];

		auto surf_nearest_point = Physics::ClosestPointOnSurface(std::get<0>(sphere), presenter.Model->Level, surface);
		auto surf_nearest_dist = Math::Length(std::get<0>(sphere) - surf_nearest_point);

		if(		// Passable:
				surf_nearest_dist <= std::get<1>(sphere) &&
				surface.Adjoin >= 0 &&
				(presenter.Model->Adjoins[surface.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement) &&
				!(presenter.Model->Adjoins[surface.Adjoin].Flags & Flags::AdjoinFlag::AllowAiOnly) &&
				!(surface.Flags & Flags::SurfaceFlag::Impassable)) {
			// Recurse into adjoined sector.
			FindSectorRestingManifolds(sphere, surface.AdjoinedSector, closed, manifolds);
		}
		else if(surf_nearest_dist <= std::get<1>(sphere)) {
			// Sphere is resting on surface.
			manifolds.push_back((std::get<0>(sphere) - surf_nearest_point) / surf_nearest_dist);
		}
	}
}

void Gorc::Game::World::Level::Gameplay::PlayerController::Update(int thing_id, double dt) {
	auto& thing = presenter.Model->Things[thing_id];

	std::vector<Math::Vector<3>> resting_manifolds;
	std::set<int> closed_set;
	FindSectorRestingManifolds(Physics::Sphere(thing.Position, thing.Size), thing.Sector, closed_set, resting_manifolds);

	Math::Vector<3> new_thing_vel = thing.Thrust;

	// Reject manifold vectors from velocity.
	for(const auto& manifold : resting_manifolds) {
		auto vel_dot = Math::Dot(new_thing_vel, manifold);
		if(vel_dot < 0.0f) {
			// Reject manifold vector from velocity.
			new_thing_vel -= vel_dot * manifold;
		}
	}

	presenter.AdjustThingPos(thing_id, thing.Position + new_thing_vel * dt);
}
