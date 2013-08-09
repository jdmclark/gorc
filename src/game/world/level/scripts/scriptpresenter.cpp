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
	for(auto& cog : model->SleepingCogs) {
		std::get<0>(cog) -= dt;
		if(std::get<0>(cog) <= 0.0) {
			model->RunningCogState.push(std::get<1>(cog));
			model->SleepingCogs.Destroy(cog);
		}
	}

	// Run sleeping cogs
	RunWaitingCogs();

	// Update timers
	for(auto& timer : model->Timers) {
		timer.Delay -= dt;
		if(timer.Delay <= 0.0) {
			SendMessage(timer.InstanceId, Cog::MessageId::Timer, timer.Id, 0, Flags::MessageType::Nothing,
					0, Flags::MessageType::Nothing, timer.Param0, timer.Param1);
			model->Timers.Destroy(timer.GetId());
		}
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::RunWaitingCogs() {
	while(!model->RunningCogState.empty()) {
		Cog::Instance& inst = *std::get<0>(model->Cogs[model->RunningCogState.top().InstanceId]);
		VirtualMachine.Execute(inst.Heap, inst.Script.Code, model->RunningCogState.top().ProgramCounter, components.VerbTable);
		model->RunningCogState.pop();
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::ResumeWaitForStop(Id<Thing> wait_thing) {
	// Enqueue waiting cogs
	for(auto& wait_cog : model->WaitForStopCogs) {
		if(std::get<0>(wait_cog) == wait_thing) {
			model->RunningCogState.push(std::get<1>(wait_cog));
			model->WaitForStopCogs.Destroy(wait_cog);
		}
	}

	RunWaitingCogs();
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::CreateLevelDummyInstance() {
	// Create an empty, non-functional COG instance for padding out the list of level instances.
	model->Cogs.emplace_back(std::unique_ptr<Cog::Instance>(nullptr), ScriptTimerState());
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

		case Cog::Symbols::SymbolType::Keyframe:
			try {
				(*jt) = manager.LoadId<Content::Assets::Animation>(static_cast<const char*>(*jt));
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

		case Cog::Symbols::SymbolType::Cog: {
			// Already an integer index.
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
			Id<Thing> index(static_cast<int>(*jt));
			if(index.IsValid()) {
				levelModel->Things[index].Flags += Flags::ThingFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Ai:
			// TODO: Handle AI loading.
		default:
			break;
		}
	}
}

Gorc::Cog::VM::Value Gorc::Game::World::Level::Scripts::ScriptPresenter::SendMessage(int InstanceId, Cog::MessageId message,
		int SenderId, int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		Cog::VM::Value Param0, Cog::VM::Value Param1, Cog::VM::Value Param2, Cog::VM::Value Param3) {
	if(InstanceId < 0) {
		return 0;
	}

	auto& instance = std::get<0>(model->Cogs[InstanceId]);
	if(instance) {
		model->RunningCogState.emplace(InstanceId, SenderId, SenderRef, SenderType, SourceRef, SourceType, Param0, Param1, Param2, Param3);

		instance->Call(components.VerbTable, VirtualMachine, message);

		Cog::VM::Value rex_val = model->RunningCogState.top().ReturnExValue;

		model->RunningCogState.pop();

		return rex_val;
	}

	return 0;
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SendMessageToAll(Cog::MessageId message,
		int SenderId, int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		Cog::VM::Value Param0, Cog::VM::Value Param1, Cog::VM::Value Param2, Cog::VM::Value Param3) {
	for(unsigned int i = 0; i < model->Cogs.size(); ++i) {
		SendMessage(i, message, SenderId, SenderRef, SenderType, SourceRef, SourceType, Param0, Param1, Param2, Param3);
	}
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SendMessageToLinked(Cog::MessageId message,
		int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		Cog::VM::Value Param0, Cog::VM::Value Param1, Cog::VM::Value Param2, Cog::VM::Value Param3) {
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
		auto& inst_ptr = std::get<0>(model->Cogs[i]);
		if(!inst_ptr) {
			continue;
		}

		Cog::Instance& inst = *inst_ptr;

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

void Gorc::Game::World::Level::Scripts::ScriptPresenter::SetTimerEx(float time, int id, Cog::VM::Value param0, Cog::VM::Value param1) {
	ScriptTimer& timer = model->Timers.Create();
	timer.InstanceId = model->RunningCogState.top().InstanceId;
	timer.Delay = time;
	timer.Id = id;
	timer.Param0 = param0;
	timer.Param1 = param1;
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::Sleep(float time) {
	ScriptContinuation continuation = model->RunningCogState.top();

	continuation.ProgramCounter = VirtualMachine.GetProgramCounter();

	auto& sleep_tuple = model->SleepingCogs.Create();
	std::get<0>(sleep_tuple) = time;
	std::get<1>(sleep_tuple) = continuation;

	VirtualMachine.Abort();
}

void Gorc::Game::World::Level::Scripts::ScriptPresenter::WaitForStop(Id<Thing> thing) {
	ScriptContinuation continuation = model->RunningCogState.top();

	continuation.ProgramCounter = VirtualMachine.GetProgramCounter();

	auto& sleep_tuple = model->WaitForStopCogs.Create();
	std::get<0>(sleep_tuple) = thing;
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

	verbTable.AddVerb<void, 1>("returnex", [&components](Cog::VM::Value value) {
		components.CurrentLevelPresenter->ScriptPresenter.model->RunningCogState.top().ReturnExValue = value;
	});

	verbTable.AddVerb<void, 2>("sendmessage", [&components](int cog_id, int message) {
		components.CurrentLevelPresenter->ScriptPresenter.SendMessage(cog_id, static_cast<Cog::MessageId>(message),
				-1, components.CurrentLevelPresenter->ScriptPresenter.model->RunningCogState.top().InstanceId, Flags::MessageType::Cog,
				static_cast<int>(components.CurrentLevelPresenter->GetLocalPlayerThing()), Flags::MessageType::Thing);
	});

	verbTable.AddVerb<Cog::VM::Value, 6>("sendmessageex", [&components](int cog_id, int message,
			Cog::VM::Value param0, Cog::VM::Value param1, Cog::VM::Value param2, Cog::VM::Value param3) {
		return components.CurrentLevelPresenter->ScriptPresenter.SendMessage(cog_id, static_cast<Cog::MessageId>(message),
				-1, components.CurrentLevelPresenter->ScriptPresenter.model->RunningCogState.top().InstanceId, Flags::MessageType::Cog,
				static_cast<int>(components.CurrentLevelPresenter->GetLocalPlayerThing()), Flags::MessageType::Thing,
				param0, param1, param2, param3);
	});

	verbTable.AddVerb<void, 1>("setpulse", [&components](float time) {
		components.CurrentLevelPresenter->ScriptPresenter.SetPulse(time);
	});

	verbTable.AddVerb<void, 1>("settimer", [&components](float time) {
		components.CurrentLevelPresenter->ScriptPresenter.SetTimer(time);
	});

	verbTable.AddVerb<void, 4>("settimerex", [&components](float time, int id, Cog::VM::Value param0, Cog::VM::Value param1) {
		components.CurrentLevelPresenter->ScriptPresenter.SetTimerEx(time, id, param0, param1);
	});

	verbTable.AddVerb<void, 1>("sleep", [&components](float time) {
		components.CurrentLevelPresenter->ScriptPresenter.Sleep(time);
	});

	verbTable.AddVerb<void, 1>("waitforstop", [&components](int thing_id) {
		components.CurrentLevelPresenter->ScriptPresenter.WaitForStop(Id<Thing>(thing_id));
	});

	verbTable.AddVerb<int, 0>("getmastercog", [&components] {
		return components.CurrentLevelPresenter->ScriptPresenter.GetMasterCog();
	});

	verbTable.AddVerb<void, 1>("setmastercog", [&components](int cog) {
		components.CurrentLevelPresenter->ScriptPresenter.SetMasterCog(cog);
	});
}
