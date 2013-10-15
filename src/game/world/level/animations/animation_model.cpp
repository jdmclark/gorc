#include "animation_model.h"

gorc::game::world::level::animations::animation_model::animation_model()
	: animations(on_animation_destroy) {
	return;
}

void gorc::game::world::level::animations::animation_model::on_animation_destroy(pool<pool_ptr<animation>>& p,
		pool<pool_ptr<animation>>::element& anim) {
	anim->stop();
}
