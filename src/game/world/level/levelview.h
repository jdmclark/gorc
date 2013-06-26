#pragma once

#include "game/view.h"
#include <btBulletDynamicsCommon.h>

namespace Gorc {

namespace Content {
namespace Assets {
class Template;
}
}

namespace Game {

class Components;

namespace World {
namespace Level {

class LevelPresenter;
class LevelModel;
class Thing;

class LevelView : public View {
private:
	LevelPresenter* currentPresenter = nullptr;
	LevelModel* currentModel = nullptr;

	class PhysicsDebugDraw : public btIDebugDraw {
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
		virtual void reportErrorWarning(const char* warningString);
		virtual void draw3dText(const btVector3& location, const char* textString);
		virtual void setDebugMode(int debugMode);
		virtual int getDebugMode() const;
	} physicsDebugDraw;

	void DrawLevel(double aspect);
	void DrawThing(const Thing& thing, double aspect);

public:
	inline void SetPresenter(LevelPresenter* presenter) {
		currentPresenter = presenter;
	}

	inline void SetLevelModel(LevelModel* levelModel) {
		currentModel = levelModel;
	}

	inline btIDebugDraw* GetPhysicsDebugDrawer() {
		return &physicsDebugDraw;
	}

	void Update(double dt);
	void Draw(double dt, const Math::Box<2, unsigned int>& view_size);

	void TranslateCamera(const Math::Vector<3>& amt);
	void YawCamera(double amt);
	void PitchCamera(double amt);
};

}
}
}
}
