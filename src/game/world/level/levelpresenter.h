#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "levelplace.h"

#include <SFML/Audio.hpp>

#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

class LevelPresenter : public Gorc::Place::Presenter {
private:
	Components& components;
	LevelPlace place;
	std::unique_ptr<LevelModel> model;
	sf::Sound AmbientSound;

	void UpdateCameraAmbientSound();
	void SetCameraCurrentSector(size_t sec_num);
	bool CameraInsideSector(const Gorc::Content::Assets::LevelSector& sec);
	bool CameraPathPassesThroughAdjoin(const Gorc::Math::Vector<3>& oldCameraPosition,
			const Gorc::Content::Assets::LevelSector& sec, const Gorc::Content::Assets::LevelSurface& surf);
	void UpdateCameraSector(const Math::Vector<3>& oldCameraPosition);

public:
	LevelPresenter(Components& components, const LevelPlace& place);

	void Start(Event::EventBus& eventBus);
	void Update(double dt);

	void TranslateCamera(const Math::Vector<3>& amt);
	void YawCamera(double amt);
	void PitchCamera(double amt);
};

}
}
}
}
