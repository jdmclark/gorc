#include "scriptpresenter.h"
#include "scriptmodel.h"
#include "game/components.h"
#include "game/world/level/levelmodel.h"

Gorc::Game::World::Level::Scripts::ScriptPresenter::ScriptPresenter(Components& components)
	: components(components), levelModel(nullptr), model(nullptr) {
	return;
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::Start(LevelModel& levelModel, ScriptModel& scriptModel) {
	this->levelModel = &levelModel;
	model = &scriptModel;
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::Update(double dt) {
	// Update cogs
	for(unsigned int i = 0; i < model->Cogs.size(); ++i) {
		auto& cog = model->Cogs[i];
		Cog::Instance& inst = *std::get<0>(cog);
		ScriptTimerState& timer_state = std::get<1>(cog);

		if(timer_state.TimerRemainingTime > 0.0) {
			timer_state.TimerRemainingTime -= dt;
			if(timer_state.TimerRemainingTime <= 0.0) {
				timer_state.TimerRemainingTime = 0.0;
				SendMessage(i, Cog::MessageId::Timer, -1, -1, Flags::MessageType::Nothing);
			}
		}

		if(timer_state.PulseTime > 0.0) {
			timer_state.PulseRemainingTime -= dt;
			if(timer_state.PulseRemainingTime <= 0.0) {
				timer_state.PulseRemainingTime = timer_state.PulseTime;
				SendMessage(i, Cog::MessageId::Pulse, -1, -1, Flags::MessageType::Nothing);
			}
		}
	}

	// Enqueue sleeping cogs
	for(auto it = model->SleepingCogs.begin(); it != model->SleepingCogs.end(); ++it) {
		std::get<0>(*it) -= dt;
		if(std::get<0>(*it) <= 0.0) {
			model->RunningCogState.push(std::get<1>(*it));
			model->SleepingCogs.Destroy(it.GetIndex());
		}
	}

	// Run sleeping cogs
	while(!model->RunningCogState.empty()) {
		Cog::Instance& inst = *std::get<0>(model->Cogs[model->RunningCogState.top().InstanceId]);
		VirtualMachine.Execute(inst.Heap, inst.Script.Code, model->RunningCogState.top().ProgramCounter, components.VerbTable);
		model->RunningCogState.pop();
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::CreateLevelCogInstance(const Cog::Script& script, Content::Manager& manager,
		Cog::Compiler& compiler, const std::vector<Cog::VM::Value>& values) {
	model->Cogs.emplace_back(std::unique_ptr<Cog::Instance>(new Cog::Instance(script)), ScriptTimerState());
	Cog::Instance& inst = *std::get<0>(model->Cogs.back());

	inst.Heap.resize(script.SymbolTable.size());

	auto it = script.SymbolTable.begin();
	auto jt = inst.Heap.begin();
	auto kt = values.begin();

	for( ; it != script.SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		if(kt != values.end() && !it->Local && it->Type != Cog::Symbols::SymbolType::Message) {
			(*jt) = *kt;
			++kt;
		}
		else {
			(*jt) = it->DefaultValue;
		}
	}

	// Load instance resources and set flags
	it = script.SymbolTable.begin();
	jt = inst.Heap.begin();

	for( ; it != script.SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		switch(it->Type) {
		case Cog::Symbols::SymbolType::Cog:
			try {
				(*jt) = manager.LoadId<Content::Assets::Script>(static_cast<const char*>(*jt), compiler);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Material:
			try {
				(*jt) = manager.LoadId<Content::Assets::Material>(static_cast<const char*>(*jt), *levelModel->Level.MasterColormap);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Model:
			try {
				(*jt) = manager.LoadId<Content::Assets::Model>(static_cast<const char*>(*jt), *levelModel->Level.MasterColormap);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Sound:
			try {
				(*jt) = manager.LoadId<Content::Assets::Sound>(static_cast<const char*>(*jt));
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Template: {
			auto it = levelModel->Level.TemplateMap.find(static_cast<const char*>(*jt));
			if(it == levelModel->Level.TemplateMap.end()) {
				// TODO: Template not found, report error.
				(*jt) = -1;
			}
			else {
				(*jt) = it->second;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Sector: {
			int index = static_cast<int>(*jt);
			if(index >= 0) {
				levelModel->Sectors[index].Flags += Flags::SectorFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Surface: {
			int index = static_cast<int>(*jt);
			if(index >= 0) {
				levelModel->Surfaces[index].Flags += Flags::SurfaceFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Thing: {
			int index = static_cast<int>(*jt);
			if(index >= 0) {
				levelModel->Things[index].Flags += Flags::ThingFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Ai:
		case Cog::Symbols::SymbolType::Keyframe:
			// TODO: Handle AI and keyframe loading.
		default:
			break;
		}
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SendMessage(unsigned int InstanceId, Cog::MessageId message,
		int SenderId, int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3) {
	model->RunningCogState.emplace(InstanceId, SenderId, SenderRef, SenderType, SourceRef, SourceType, Param0, Param1, Param2, Param3);

	std::get<0>(model->Cogs[InstanceId])->Call(components.VerbTable, VirtualMachine, message);

	model->RunningCogState.pop();
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SendMessageToAll(Cog::MessageId message,
		int SenderId, int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3) {
	for(unsigned int i = 0; i < model->Cogs.size(); ++i) {
		SendMessage(i, message, SenderId, SenderRef, SenderType,
				SourceRef, SourceType, Param0, Param1, Param2, Param3);
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SendMessageToLinked(Cog::MessageId message,
		int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3) {
	Cog::Symbols::SymbolType expectedSymbolType;

	switch(SenderType) {
	case Flags::MessageType::Sector:
		expectedSymbolType = Cog::Symbols::SymbolType::Sector;
		break;

	case Flags::MessageType::Surface:
		expectedSymbolType = Cog::Symbols::SymbolType::Surface;
		break;

	case Flags::MessageType::Thing:
		expectedSymbolType = Cog::Symbols::SymbolType::Thing;
		break;
	}

	for(unsigned int i = 0; i < model->Cogs.size(); ++i) {
		Cog::Instance& inst = *std::get<0>(model->Cogs[i]);

		auto it = inst.Script.SymbolTable.begin();
		auto jt = inst.Heap.begin();

		for(; it != inst.Script.SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
			if(it->Type == expectedSymbolType && static_cast<int>(*jt) == SenderRef) {
				SendMessage(i, message,
						it->Linkid, SenderRef, SenderType, SourceRef, SourceType,
						Param0, Param1, Param2, Param3);
			}
		}
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SetPulse(float time) {
	ScriptTimerState& state = std::get<1>(model->Cogs[model->RunningCogState.top().InstanceId]);
	state.PulseTime = time;
	state.PulseRemainingTime = time;
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SetTimer(float time) {
	std::get<1>(model->Cogs[model->RunningCogState.top().InstanceId]).TimerRemainingTime = time;
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::Sleep(float time) {
	ScriptContinuation continuation = model->RunningCogState.top();

	continuation.ProgramCounter = VirtualMachine.GetProgramCounter();

	auto& sleep_tuple = *std::get<0>(model->SleepingCogs.Create());
	std::get<0>(sleep_tuple) = time;
	std::get<1>(sleep_tuple) = continuation;

	VirtualMachine.Abort();
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<int, 1>("getparam", [&components](int param_num) {
		return components.CurrentLevelPresenter->ScriptPresenter.GetParam(param_num);
	});

	verbTable.AddVerb<int, 0>("getsenderid", [&components]{
		return components.CurrentLevelPresenter->ScriptPresenter.GetSenderId();
	});

	verbTable.AddVerb<int, 0>("getsenderref", [&components]{
		return components.CurrentLevelPresenter->ScriptPresenter.GetSenderRef();
	});

	verbTable.AddVerb<int, 0>("getsendertype", [&components]{
		return components.CurrentLevelPresenter->ScriptPresenter.GetSenderType();
	});

	verbTable.AddVerb<int, 0>("getsourceref", [&components]{
		return components.CurrentLevelPresenter->ScriptPresenter.GetSourceRef();
	});

	verbTable.AddVerb<int, 0>("getsourcetype", [&components]{
		return components.CurrentLevelPresenter->ScriptPresenter.GetSourceType();
	});

	verbTable.AddVerb<void, 1>("setpulse", [&components](float time) {
		components.CurrentLevelPresenter->ScriptPresenter.SetPulse(time);
	});

	verbTable.AddVerb<void, 1>("settimer", [&components](float time) {
		components.CurrentLevelPresenter->ScriptPresenter.SetTimer(time);
	});

	verbTable.AddVerb<void, 1>("sleep", [&components](float time) {
		components.CurrentLevelPresenter->ScriptPresenter.Sleep(time);
	});
}
