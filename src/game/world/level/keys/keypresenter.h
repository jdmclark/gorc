#pragma once

#include "content/flags/keyflag.h"
#include "framework/flagset.h"
#include "framework/math/vector.h"

namespace Gorc {

namespace Cog {
namespace Verbs {
class VerbTable;
}
}

namespace Content {
class Manager;
}

namespace Game {

class Components;

namespace World {
namespace Level {

class LevelModel;

namespace Keys {

class KeyModel;

class KeyPresenter {
private:
	Content::Manager& contentManager;
	LevelModel* levelModel;
	KeyModel* model;

public:
	KeyPresenter(Content::Manager& contentManager);

	void Start(LevelModel& levelModel, KeyModel& model);
	void Update(double dt);

	int PlayKey(int thing_id, int key, int priority, FlagSet<Flags::KeyFlag> flags);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
