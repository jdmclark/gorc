#include "cog/verbs/table.h"
#include "game/components.h"
#include "keypresenter.h"
#include "keymodel.h"
#include "game/world/level/levelmodel.h"
#include "content/manager.h"
#include "game/world/level/gameplay/charactercontroller.h"

Gorc::Game::World::Level::Keys::KeyPresenter::KeyPresenter(Content::Manager& contentManager)
	: contentManager(contentManager), levelModel(nullptr), model(nullptr) {
	return;
}

void Gorc::Game::World::Level::Keys::KeyPresenter::Start(LevelModel& levelModel, KeyModel& model) {
	this->levelModel = &levelModel;
	this->model = &model;
}

void Gorc::Game::World::Level::Keys::KeyPresenter::DispatchAllMarkers(int thing_id, const std::vector<std::tuple<double, Flags::KeyMarkerType>>& markers,
		double begin, double end, bool wraps, double frame_ct) {
	if(wraps) {
		begin = std::fmod(begin, frame_ct);
		end = std::fmod(end, frame_ct);
	}

	if(wraps && begin > end) {
		for(const auto& marker : markers) {
			double ft = std::get<0>(marker);
			if(ft >= begin || ft < end) {
				DispatchMarker(thing_id, std::get<1>(marker));
			}
		}
	}
	else {
		for(const auto& marker : markers) {
			double ft = std::get<0>(marker);
			if(ft >= begin && ft < end) {
				DispatchMarker(thing_id, std::get<1>(marker));
			}
		}
	}
}

void Gorc::Game::World::Level::Keys::KeyPresenter::DispatchMarker(int thing_id, Flags::KeyMarkerType marker) {
	auto& thing = levelModel->Things[thing_id];
	thing.Controller->HandleAnimationMarker(thing_id, marker);
}

int Gorc::Game::World::Level::Keys::KeyPresenter::GetThingMixId(int thing_id) {
	auto& thing = levelModel->Things[thing_id];
	if(thing.AttachedKeyMix < 0) {
		auto& mix = model->Mixes.Create();
		mix.AttachedThing = thing_id;
		thing.AttachedKeyMix = mix.GetId();
	}

	return thing.AttachedKeyMix;
}

void Gorc::Game::World::Level::Keys::KeyPresenter::Update(double dt) {
	// Reset mix priorities.
	for(auto& mix : model->Mixes) {
		mix.Body.Priority = mix.High.Priority = mix.Low.Priority = std::numeric_limits<int>::lowest();
	}

	// Update animation frames
	for(auto& key : model->Keys) {
		auto& mix = model->Mixes[key.MixId];

		// Update anim time and compute frame number
		double prev_anim_time = key.AnimationTime;
		key.AnimationTime += dt * key.Speed;

		if(key.Animation) {
			bool loops = false;
			const auto& anim = *key.Animation;
			double prev_logical_frame = anim.FrameRate * prev_anim_time;
			double logical_frame = anim.FrameRate * key.AnimationTime;
			double frame = logical_frame;

			if(key.Flags & Flags::KeyFlag::PausesOnFirstFrame) {
				if(frame > anim.FrameCount) {
					frame = 0.0;
				}
			}
			else if(key.Flags & Flags::KeyFlag::PausesOnLastFrame) {
				if(frame > anim.FrameCount) {
					frame = anim.FrameCount;
				}
			}
			else if(static_cast<uint32_t>(key.Flags) == 0) {
				loops = true;
				frame = std::fmod(frame, anim.FrameCount);
			}

			key.CurrentFrame = frame;

			DispatchAllMarkers(mix.AttachedThing, key.Animation->Markers, prev_logical_frame, logical_frame, loops, key.Animation->FrameCount);

			if((key.Flags & Flags::KeyFlag::EndSmoothly) && frame >= anim.FrameCount) {
				// End smoothly, continue into next animation.
				model->Keys.Destroy(key);
				continue;
			}
		}

		// Apply mix
		if(key.HighPriority >= mix.High.Priority) {
			mix.High.Animation = key.Animation;
			mix.High.Frame = key.CurrentFrame;
			mix.High.Priority = key.HighPriority;
		}

		if(key.LowPriority >= mix.Low.Priority) {
			mix.Low.Animation = key.Animation;
			mix.Low.Frame = key.CurrentFrame;
			mix.Low.Priority = key.LowPriority;
		}

		if(key.BodyPriority >= mix.Body.Priority) {
			mix.Body.Animation = key.Animation;
			mix.Body.Frame = key.CurrentFrame;
			mix.Body.Priority = key.BodyPriority;
		}
	}
}

std::tuple<Gorc::Math::Vector<3>, Gorc::Math::Vector<3>> Gorc::Game::World::Level::Keys::KeyPresenter::GetNodeFrame(int mix_id,
		int node_id, FlagSet<Flags::MeshNodeType> node_type) const {
	const KeyMix& mix = model->Mixes[mix_id];

	// Get animation corresponding to node type
	const KeyMixLevelState* mix_level;
	if(node_type & Flags::MeshNodeType::UpperBody) {
		mix_level = &mix.High;
	}
	else if(node_type & Flags::MeshNodeType::LowerBody) {
		mix_level = &mix.Low;
	}
	else {
		mix_level = &mix.Body;
	}

	if(!mix_level->Animation || mix_level->Animation->Nodes.size() <= node_id ||
			mix_level->Animation->Nodes[node_id].Frames.empty()) {
		// Abort if there are no frames to interpolate.
		return std::make_tuple(Math::Zero<3>(), Math::Zero<3>());
	}

	const auto& anim_node = mix_level->Animation->Nodes[node_id];

	int actual_frame = static_cast<int>(std::floor(mix_level->Frame));

	// Convert anim_time into a frame number
	auto comp_fn = [](int tgt_fr, const Content::Assets::AnimationFrame& fr) {
		return fr.Frame > tgt_fr;
	};

	// Find frame immediately after desired frame, then back off.
	auto it = std::upper_bound(anim_node.Frames.begin(), anim_node.Frames.end(), actual_frame, comp_fn);
	if(it == anim_node.Frames.begin()) {
		it = anim_node.Frames.end() - 1;
	}
	else {
		--it;
	}

	float remaining_frame_time = static_cast<float>(mix_level->Frame) - static_cast<float>(it->Frame);

	auto position = it->Position + remaining_frame_time * it->DeltaPosition;
	auto orientation = it->Orientation + remaining_frame_time * it->DeltaOrientation;

	return std::make_tuple(position, orientation);
}

int Gorc::Game::World::Level::Keys::KeyPresenter::PlayKey(int thing_id, int key,
		int priority, FlagSet<Flags::KeyFlag> flags) {
	int mix_id = GetThingMixId(thing_id);

	auto& state = model->Keys.Create();

	state.Animation = &contentManager.GetAsset<Content::Assets::Animation>(key);
	state.HighPriority = state.LowPriority = state.HighPriority = priority;
	state.AnimationTime = 0.0;
	state.CurrentFrame = 0.0;
	state.MixId = mix_id;
	state.Flags = flags;
	state.Speed = 1.0;

	return state.GetId();
}

int Gorc::Game::World::Level::Keys::KeyPresenter::PlayPuppetKey(int thing_id,
		Flags::PuppetModeType major_mode, Flags::PuppetSubmodeType minor_mode) {
	int mix_id = GetThingMixId(thing_id);
	auto& thing = levelModel->Things[thing_id];

	auto& state = model->Keys.Create();

	const Content::Assets::PuppetSubmode& submode = thing.Puppet->GetMode(major_mode).GetSubmode(minor_mode);

	state.Animation = submode.Animation;
	state.HighPriority = submode.HiPriority;
	state.LowPriority = submode.LoPriority;
	state.BodyPriority = std::max(submode.HiPriority, submode.LoPriority);
	state.AnimationTime = 0.0;
	state.CurrentFrame = 0.0;
	state.MixId = mix_id;
	state.Flags = submode.Flags;
	state.Speed = 1.0;

	return state.GetId();
}

void Gorc::Game::World::Level::Keys::KeyPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<int, 4>("playkey", [&components](int thing, int key, int priority, int flags) {
		return static_cast<int>(components.CurrentLevelPresenter->KeyPresenter.PlayKey(thing, key, priority, FlagSet<Flags::KeyFlag>(flags)));
	});
}
