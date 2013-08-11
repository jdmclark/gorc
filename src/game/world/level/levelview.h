#pragma once

#include "game/view.h"
#include "framework/math/box.h"
#include "framework/math/matrix.h"
#include "framework/flagset.h"
#include "content/flags/keyflag.h"
#include "levelshader.h"
#include <stack>
#include <unordered_set>
#include <btBulletDynamicsCommon.h>

namespace Gorc {

namespace Cog {
namespace Verbs {
class VerbTable;
}
}

namespace Content {
class Manager;

namespace Assets {
class Template;
class LevelSector;
class Shader;
class Model;
class Animation;
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
	SurfaceShader surfaceShader;
	HorizonShader horizonShader;
	CeilingShader ceilingShader;
	LightShader lightShader;

	LevelShader* currentLevelShader = nullptr;

	Math::Matrix<float> ProjectionMatrix;
	Math::Matrix<float> ViewMatrix;
	std::stack<Math::Matrix<float>> ModelMatrixStack;

	template <typename T, typename... U> void SetCurrentShader(T& shader, U... args) {
		currentLevelShader = &shader;
		shader.Activate(args...);
		shader.SetProjectionMatrix(ProjectionMatrix);
		shader.SetViewMatrix(ViewMatrix);
		shader.SetModelMatrix(ModelMatrixStack.top());
	}

	inline void UpdateShaderModelMatrix() {
		currentLevelShader->SetModelMatrix(ModelMatrixStack.top());
	}

	inline void ConcatenateMatrix(const Math::Matrix<float>& mat) {
		ModelMatrixStack.top() = ModelMatrixStack.top() * mat;
	}

	void PushMatrix() {
		ModelMatrixStack.push(ModelMatrixStack.top());
	}

	void PopMatrix() {
		ModelMatrixStack.pop();
	}

	LevelPresenter* currentPresenter = nullptr;
	LevelModel* currentModel = nullptr;
	std::unordered_set<unsigned int> sector_vis_scratch;
	std::unordered_set<unsigned int> sector_visited_scratch;
	std::vector<std::tuple<unsigned int, unsigned int>> horizon_sky_surfaces_scratch;
	std::vector<std::tuple<unsigned int, unsigned int>> ceiling_sky_surfaces_scratch;
	std::vector<std::tuple<unsigned int, unsigned int, float>> translucent_surfaces_scratch;
	std::vector<std::tuple<int, float>> visible_thing_scratch;

	class PhysicsDebugDraw : public btIDebugDraw {
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
		virtual void reportErrorWarning(const char* warningString);
		virtual void draw3dText(const btVector3& location, const char* textString);
		virtual void setDebugMode(int debugMode);
		virtual int getDebugMode() const;
	} physicsDebugDraw;

	void ComputeVisibleSectors(const Math::Box<2, unsigned int>& view_size);
	void RecordVisibleSpecialSurfaces();
	void RecordVisibleThings();
	void DoSectorVis(unsigned int sec_num, const std::array<double, 16>& proj_mat, const std::array<double, 16>& view_mat,
			const std::array<int, 4>& viewport, const Math::Box<2, double>& adj_bbox, const Math::Vector<3>& cam_pos, const Math::Vector<3>& cam_look);
	void DrawVisibleDiffuseSurfaces();
	void DrawVisibleSkySurfaces(const Math::Box<2, unsigned int>& screen_size);
	void DrawVisibleTranslucentSurfacesAndThings();

	void DrawSurface(unsigned int surf_num, const Content::Assets::LevelSector& sector, float alpha);
	void DrawMeshNode(const Thing& thing, const Content::Assets::Model& model, int node_id, float sector_light);
	void DrawThing(const Thing& thing);

public:
	LevelView(Content::Manager& shaderContentManager);

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
