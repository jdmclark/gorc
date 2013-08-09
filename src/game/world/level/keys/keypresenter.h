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
class KeyState;
class KeyMix;

class KeyPresenter {
private:
	Content::Manager& contentManager;
	LevelModel* levelModel;
	KeyModel* model;

	Id<KeyMix> GetThingMixId(Id<Thing> thing_id);
	void DispatchAllMarkers(Id<Thing> thing_id, const std::vector<std::tuple<double, Flags::KeyMarkerType>>& markers,
			double begin, double end, bool wraps, double frame_ct);
	void DispatchMarker(Id<Thing> thing_id, Flags::KeyMarkerType marker);

public:
	KeyPresenter(Content::Manager& contentManager);

	void Start(LevelModel& levelModel, KeyModel& model);
	void Update(double dt);

	std::tuple<Math::Vector<3>, Math::Vector<3>> GetNodeFrame(Id<KeyMix> mix_id, int node_id, FlagSet<Flags::MeshNodeType> node_type) const;

	Id<KeyState> PlayKey(Id<Thing> thing_id, int key, int priority, FlagSet<Flags::KeyFlag> flags);
	Id<KeyState> PlayPuppetKey(Id<Thing> thing_id, Flags::PuppetModeType major_mode, Flags::PuppetSubmodeType minor_mode);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
