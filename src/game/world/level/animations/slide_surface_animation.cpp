#include "slide_surface_animation.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"

using namespace Gorc::Math;

Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::SlideSurfaceAnimation(LevelModel& model, unsigned int surface,
		const Math::Vector<3>& direction, int anim_num)
	: model(model), surface(surface), direction(direction) {
	auto& surf = model.Surfaces[surface];
	surf.AnimNumber = anim_num;

	// Compute texture basis.
	auto dnsb0 = model.Level.Vertices[std::get<0>(surf.Vertices[1])] - model.Level.Vertices[std::get<0>(surf.Vertices[0])];

	sb0 = dnsb0 / Length2(dnsb0);
	sb1 = Cross(surf.Normal, sb0);

	unsigned int noncol_vert;
	for(noncol_vert = 2; noncol_vert < surf.Vertices.size(); ++noncol_vert) {
		auto sb2 = model.Level.Vertices[std::get<0>(surf.Vertices[noncol_vert])] - model.Level.Vertices[std::get<0>(surf.Vertices[0])];
		if(Dot(sb1, sb2) != 0.0f) {
			break;
		}
	}

	auto vb0 = model.Level.TextureVertices[std::get<1>(surf.Vertices[1])] - model.Level.TextureVertices[std::get<1>(surf.Vertices[0])];
	auto vb1 = Vec(Get<1>(vb0), -Get<0>(vb0));

	float sgn = 1.0f;
	if(Dot(vb1, model.Level.TextureVertices[std::get<1>(surf.Vertices[noncol_vert])] - model.Level.TextureVertices[std::get<1>(surf.Vertices[0])]) < 0.0f) {
		sgn = -1.0f;
	}

	tb0 = -vb0;
	tb1 = -sgn * vb1;

	return;
}

void Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::Update(double dt) {
	auto& surf = model.Surfaces[surface];
	surf.Thrust = direction * RateFactor;

	auto plane_dir = tb0 * Dot(direction, sb0) + tb1 * Dot(direction, sb1);

	surf.TextureOffset += plane_dir * dt * RateFactor;
}

void Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::Stop() {
	auto& surf = model.Surfaces[surface];
	surf.AnimNumber = -1;
	surf.Thrust = Math::Zero<3>();
}
