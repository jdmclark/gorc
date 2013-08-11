#include "slidesurfaceanimation.h"
#include "game/world/level/levelmodel.h"

using namespace Gorc::Math;

Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::SlideSurfaceAnimation(LevelModel& model, unsigned int surface,
		const Math::Vector<3>& direction, int anim_num)
	: model(model), surface(surface), direction(direction) {
	auto& surf = model.Surfaces[surface];
	surf.AnimNumber = anim_num;

	// Compute texture basis.
	sb0 = Normalize(model.Level.Vertices[std::get<0>(surf.Vertices[1])] - model.Level.Vertices[std::get<0>(surf.Vertices[0])]);
	sb1 = Cross(surf.Normal, sb0);

	unsigned int noncol_vert;
	for(int i = 2; i < surf.Vertices.size(); ++i) {
		noncol_vert = i;
		auto sb2 = model.Level.Vertices[std::get<0>(surf.Vertices[i])] - model.Level.Vertices[std::get<0>(surf.Vertices[0])];
		if(Dot(sb1, sb2) != 0.0f) {
			break;
		}
	}

	auto vb0 = Normalize(model.Level.TextureVertices[std::get<1>(surf.Vertices[1])] - model.Level.TextureVertices[std::get<1>(surf.Vertices[0])]);
	auto vb1 = Vec(Get<1>(vb0), -Get<0>(vb0));

	if(Dot(vb1, model.Level.TextureVertices[std::get<1>(surf.Vertices[noncol_vert])] - model.Level.TextureVertices[std::get<1>(surf.Vertices[0])]) < 0.0f) {
		vb1 = -vb1;
	}

	float det = -1.0f / (Get<0>(vb0) * Get<1>(vb1) - Get<0>(vb1) * Get<1>(vb0));
	tb0 = det * Vec(Get<1>(vb1), -Get<0>(vb1));
	tb1 = det * Vec(-Get<1>(vb0), Get<0>(vb0));

	return;
}

void Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::Update(double dt) {
	auto& surf = model.Surfaces[surface];
	surf.Thrust = direction;

	auto plane_dir = tb0 * Dot(direction, sb0) + tb1 * Dot(direction, sb1);

	surf.TextureOffset += plane_dir / 8.0f;
}

void Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::Stop() {
	auto& surf = model.Surfaces[surface];
	surf.AnimNumber = -1;
	surf.Thrust = Math::Zero<3>();
}
