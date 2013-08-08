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

	unsigned int GetThingMixId(int thing_id);
	void DispatchAllMarkers(int thing_id, const std::vector<std::tuple<double, Flags::KeyMarkerType>>& markers,
			double begin, double end, bool wraps, double frame_ct);
	void DispatchMarker(int thing_id, Flags::KeyMarkerType marker);

public:
	KeyPresenter(Content::Manager& contentManager);

	void Start(LevelModel& levelModel, KeyModel& model);
	void Update(double dt);

	std::tuple<Math::Vector<3>, Math::Vector<3>> GetNodeFrame(int mix_id, int node_id, FlagSet<Flags::MeshNodeType> node_type) const;

	int PlayKey(int thing_id, int key, int priority, FlagSet<Flags::KeyFlag> flags);
	int PlayPuppetKey(int thing_id, Flags::PuppetModeType major_mode, Flags::PuppetSubmodeType minor_mode);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
