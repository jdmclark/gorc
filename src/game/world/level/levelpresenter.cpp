#include "levelpresenter.h"
#include "levelmodel.h"

using namespace Gorc::Math;

Gorc::Game::World::Level::LevelPresenter::LevelPresenter(Components& components, const LevelPlace& place)
	: components(components), place(place) {
	return;
}

void Gorc::Game::World::Level::LevelPresenter::Start(Event::EventBus& eventBus) {
	model = std::unique_ptr<LevelModel>(new LevelModel(place.Level));

	components.LevelView.SetPresenter(this);
	components.LevelView.SetLevelModel(model.get());
	components.WorldViewFrame.SetView(components.LevelView);

	UpdateCameraAmbientSound();

	return;
}

void Gorc::Game::World::Level::LevelPresenter::Update(double dt) {
	model->CameraRigidBody->applyCentralForce(BtVec(model->CameraVelocity * 800.0f));
	model->CameraRigidBody->setDamping(0.999f, 0.99f);

	model->DynamicsWorld.stepSimulation(dt, 10);

	auto OldCameraPosition = model->CameraPosition;
	btTransform trans;
	model->CameraRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 cam_origin = trans.getOrigin();

	model->CameraPosition = VecBt(cam_origin);
	UpdateCameraSector(OldCameraPosition);
}

void Gorc::Game::World::Level::LevelPresenter::UpdateCameraAmbientSound() {
	const auto& sec = model->Level.Sectors[model->CameraCurrentSector];
	if(&sec.AmbientSound->Buffer == AmbientSound.GetBuffer() && AmbientSound.GetStatus() != sf::Sound::Stopped) {
		AmbientSound.SetVolume(sec.AmbientSoundVolume * 100.0f);
	}
	else if (sec.AmbientSound != nullptr) {
		AmbientSound.SetBuffer(sec.AmbientSound->Buffer);
		AmbientSound.SetLoop(true);
		AmbientSound.SetVolume(sec.AmbientSoundVolume * 100.0f);
		AmbientSound.Play();
	}
	else {
		AmbientSound.Stop();
	}
}

void Gorc::Game::World::Level::LevelPresenter::SetCameraCurrentSector(size_t sec_num) {
	if(model->CameraCurrentSector != sec_num) {
		model->CameraCurrentSector = sec_num;
		UpdateCameraAmbientSound();
	}
}

bool Gorc::Game::World::Level::LevelPresenter::CameraInsideSector(const Gorc::Content::Assets::LevelSector& sec) {
	for(size_t i =  sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
		const auto& surf = model->Level.Surfaces[i];
		const auto& p = model->Level.Vertices[std::get<0>(surf.Vertices[0])];

		if(Dot(surf.Normal, model->CameraPosition - p) < 0.0f) {
			return false;
		}
	}

	return true;
}

bool Gorc::Game::World::Level::LevelPresenter::CameraPathPassesThroughAdjoin(const Vector<3>& oldCameraPosition,
		const Gorc::Content::Assets::LevelSector& sec, const Gorc::Content::Assets::LevelSurface& surf) {
	if(Dot(model->CameraPosition - oldCameraPosition, surf.Normal) > 0.0f) {
		return false;
	}

	auto p = model->Level.Vertices[std::get<0>(surf.Vertices[0])];
	auto u = Dot(surf.Normal, p - oldCameraPosition) / Dot(surf.Normal, model->CameraPosition - oldCameraPosition);
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	return true;
}

void Gorc::Game::World::Level::LevelPresenter::UpdateCameraSector(const Vector<3>& oldCameraPosition) {
	std::vector<size_t> sectors { model->CameraCurrentSector };
	while(!sectors.empty()) {
		size_t sec_num = sectors.back();
		sectors.pop_back();

		const auto& sec = model->Level.Sectors[sec_num];

		if(CameraInsideSector(sec)) {
			SetCameraCurrentSector(sec.Number);
			return;
		}

		for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
			const auto& surf = model->Level.Surfaces[i];
			if(surf.Adjoin >= 0 && !(surf.Flags & Gorc::Content::Assets::SurfaceFlag::Impassable)
					&& CameraPathPassesThroughAdjoin(oldCameraPosition, sec, surf)) {
				sectors.push_back(surf.AdjoinedSector);
			}
		}
	}
}

void Gorc::Game::World::Level::LevelPresenter::TranslateCamera(const Vector<3>& amt) {
	model->CameraVelocity = Zero<3>();
	model->CameraVelocity += Get<X>(amt) * Cross(model->CameraLook, model->CameraUp);
	model->CameraVelocity += Get<Z>(amt) * model->CameraUp;
	model->CameraVelocity += Get<Y>(amt) * model->CameraLook;
}

void Gorc::Game::World::Level::LevelPresenter::YawCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewLook = Vec(
			cost * Get<X>(model->CameraLook) - sint * Get<Y>(model->CameraLook),
			sint * Get<X>(model->CameraLook) + cost * Get<Y>(model->CameraLook),
			Get<Z>(model->CameraLook));
	Vector<3> NewUp = Vec(
			cost * Get<X>(model->CameraUp) - sint * Get<Y>(model->CameraUp),
			sint * Get<X>(model->CameraUp) + cost * Get<Y>(model->CameraUp),
			Get<Z>(model->CameraUp));

	model->CameraLook = Normalize(NewLook);
	model->CameraUp = Normalize(NewUp);
}

void Gorc::Game::World::Level::LevelPresenter::PitchCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewUp = (-sint * model->CameraLook) + (cost * model->CameraUp);
	Vector<3> NewLook = (cost * model->CameraLook) + (sint * model->CameraUp);
	model->CameraUp = Normalize(NewUp);
	model->CameraLook = Normalize(NewLook);
}
