#include "script.h"
#include "model.h"
#include "sound.h"
#include "material.h"
#include "content/manager.h"

void Gorc::Content::Assets::Script::LoadInstanceResources(Cog::Instance& inst, Manager& manager, Cog::Compiler& compiler,
		const Assets::Colormap& colormap, const std::unordered_map<std::string, int>& template_map) const {
	auto it = Script.SymbolTable.begin();
	auto jt = inst.Heap.begin();

	for( ; it != Script.SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		switch(it->Type) {
		case Cog::Symbols::SymbolType::Cog:
			try {
				(*jt) = manager.LoadId<Assets::Script>(static_cast<const char*>(*jt), compiler);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Material:
			try {
				(*jt) = manager.LoadId<Material>(static_cast<const char*>(*jt), colormap);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Model:
			try {
				(*jt) = manager.LoadId<Model>(static_cast<const char*>(*jt), colormap);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Sound:
			try {
				(*jt) = manager.LoadId<Sound>(static_cast<const char*>(*jt));
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Template: {
			auto it = template_map.find(static_cast<const char*>(*jt));
			if(it == template_map.end()) {
				// TODO: Template not found, report error.
				(*jt) = -1;
			}
			else {
				(*jt) = it->second;
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

std::unique_ptr<Gorc::Cog::Instance> Gorc::Content::Assets::Script::CreateInstance(Manager& manager, Cog::Compiler& compiler,
		const Assets::Colormap& colormap, const std::unordered_map<std::string, int>& template_map) const {
	std::unique_ptr<Cog::Instance> inst(new Cog::Instance(Script));

	inst->Heap.resize(Script.SymbolTable.size());

	auto it = Script.SymbolTable.begin();
	auto jt = inst->Heap.begin();

	for( ; it != Script.SymbolTable.end() && jt != inst->Heap.end(); ++it, ++jt) {
		(*jt) = it->DefaultValue;
	}

	LoadInstanceResources(*inst, manager, compiler, colormap, template_map);
	return std::move(inst);
}

std::unique_ptr<Gorc::Cog::Instance> Gorc::Content::Assets::Script::CreateInstance(Manager& manager, Cog::Compiler& compiler,
		const Assets::Colormap& colormap, const std::unordered_map<std::string, int>& template_map,
		const std::vector<Cog::VM::Value>& values) const {
	std::unique_ptr<Cog::Instance> inst(new Cog::Instance(Script));

	inst->Heap.resize(Script.SymbolTable.size());

	auto it = Script.SymbolTable.begin();
	auto jt = inst->Heap.begin();
	auto kt = values.begin();

	for( ; it != Script.SymbolTable.end() && jt != inst->Heap.end(); ++it, ++jt) {
		if(kt != values.end() && !it->Local && it->Type != Cog::Symbols::SymbolType::Message) {
			(*jt) = *kt;
			++kt;
		}
		else {
			(*jt) = it->DefaultValue;
		}
	}

	LoadInstanceResources(*inst, manager, compiler, colormap, template_map);
	return std::move(inst);
}
