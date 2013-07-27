#include "cog/verbs/table.h"
#include "game/components.h"
#include "keypresenter.h"
#include "keymodel.h"
#include "game/world/level/levelmodel.h"
#include "content/manager.h"

Gorc::Game::World::Level::Keys::KeyPresenter::KeyPresenter(Content::Manager& contentManager)
	: contentManager(contentManager), levelModel(nullptr), model(nullptr) {
	return;
}

void Gorc::Game::World::Level::Keys::KeyPresenter::Start(LevelModel& levelModel, KeyModel& model) {
	this->levelModel = &levelModel;
	this->model = &model;
}

void Gorc::Game::World::Level::Keys::KeyPresenter::Update(double dt) {
	for(auto& entity : model->Mixes) {
		entity.AnimationTime += dt;
	}
}

int Gorc::Game::World::Level::Keys::KeyPresenter::PlayKey(int thing_id, int key, int priority, FlagSet<Flags::KeyFlag> keys) {
	auto ent_tuple = model->Mixes.Create();

	KeyMix& mix = *std::get<0>(ent_tuple);
	mix.Animation = &contentManager.GetAsset<Content::Assets::Animation>(key);
	mix.AnimationTime = 0.0;
	mix.Playing = true;

	levelModel->Things[thing_id].AttachedKeyMix = std::get<1>(ent_tuple);

	return std::get<1>(ent_tuple);
}

void Gorc::Game::World::Level::Keys::KeyPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<int, 4>("playkey", [&components](int thing, int key, int priority, int flags) {
		return components.CurrentLevelPresenter->KeyPresenter.PlayKey(thing, key, priority, FlagSet<Flags::KeyFlag>(flags));
	});
}
