#include "animation_model.h"

Gorc::Game::World::Level::Animations::AnimationModel::AnimationModel()
	: Animations(OnAnimationDestroy) {
	return;
}

void Gorc::Game::World::Level::Animations::AnimationModel::OnAnimationDestroy(Pool<PoolPtr<Animation>>& pool,
		Pool<PoolPtr<Animation>>::Element& anim) {
	anim->Stop();
}
