#include "animationmodel.h"

Gorc::Game::World::Level::Animations::AnimationModel::AnimationModel()
	: Animations(OnAnimationDestroy) {
	return;
}

void Gorc::Game::World::Level::Animations::AnimationModel::OnAnimationDestroy(Pool<std::unique_ptr<Animation>>& pool,
		unsigned int index, std::unique_ptr<Animation>& anim) {
	anim->Stop();
}
