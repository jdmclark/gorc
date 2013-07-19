#pragma once

#include "game/view.h"
#include "framework/math/box.h"
#include <unordered_set>
#include <btBulletDynamicsCommon.h>

namespace Gorc {

namespace Cog {
namespace Verbs {
class VerbTable;
}
}

namespace Content {
namespace Assets {
class Template;
class LevelSector;
class Shader;
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
	const Content::Assets::Shader& surfaceShader;
	const Content::Assets::Shader& horizonShader;

	LevelPresenter* currentPresenter = nullptr;
	LevelModel* currentModel = nullptr;
	std::unordered_set<unsigned int> sector_vis_scratch;
	std::unordered_set<unsigned int> sector_visited_scratch;
	std::vector<std::tuple<unsigned int, unsigned int>> horizon_sky_surfaces_scratch;
	std::vector<std::tuple<unsigned int, unsigned int>> ceiling_sky_surfaces_scratch;
	std::vector<std::tuple<unsigned int, unsigned int, float>> translucent_surfaces_scratch;

	class PhysicsDebugDraw : public btIDebugDraw {
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
		virtual void reportErrorWarning(const char* warningString);
		virtual void draw3dText(const btVector3& location, const char* textString);
		virtual void setDebugMode(int debugMode);
		virtual int getDebugMode() const;
	} physicsDebugDraw;

	void SetUpViewProjectionMatrices(const Math::Box<2, unsigned int>& view_size);
	void ComputeVisibleSectors(const Math::Box<2, unsigned int>& view_size);
	void DoSectorVis(unsigned int sec_num, const std::array<double, 16>& proj_mat, const std::array<double, 16>& view_mat,
			const std::array<int, 4>& viewport, const Math::Box<2, double>& adj_bbox, const Math::Vector<3>& cam_pos, const Math::Vector<3>& cam_look);
	void ActivateSurfaceShader();
	void ActivateHorizonShader(const Math::Box<2, unsigned int>& screen_size);
	void DrawVisibleDiffuseSurfaces();
	void DrawVisibleThings();
	void DrawVisibleTranslucentSurfaces();
	void DrawVisibleSkySurfaces(const Math::Box<2, unsigned int>& screen_size);

	void DrawSurface(unsigned int surf_num, const Content::Assets::LevelSector& sector, float alpha);
	void DrawThing(const Thing& thing);

public:
	LevelView(const Content::Assets::Shader& surfaceShader, const Content::Assets::Shader& horizonShader);

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
};

}
}
}
}
