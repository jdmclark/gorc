#include "script.h"

Gorc::Cog::Script::Script() {
	return;
}

std::unique_ptr<Gorc::Cog::Instance> Gorc::Cog::Script::CreateInstance() const {
	std::unique_ptr<Instance> inst(new Instance(*this));

	inst->Heap.resize(SymbolTable.size());

	auto it = SymbolTable.begin();
	auto jt = inst->Heap.begin();

	for( ; it != SymbolTable.end() && jt != inst->Heap.end(); ++it, ++jt) {
		(*jt) = it->DefaultValue;
	}

	return std::move(inst);
}

std::unique_ptr<Gorc::Cog::Instance> Gorc::Cog::Script::CreateInstance(const std::vector<VM::Value>& values) const {
	std::unique_ptr<Instance> inst(new Instance(*this));

	inst->Heap.resize(SymbolTable.size());

	auto it = SymbolTable.begin();
	auto jt = inst->Heap.begin();
	auto kt = values.begin();

	for( ; it != SymbolTable.end() && jt != inst->Heap.end(); ++it, ++jt) {
		if(kt != values.end() && !it->Local && it->Type != Symbols::SymbolType::Message) {
			(*jt) = *kt;
			++kt;
		}
		else {
			(*jt) = it->DefaultValue;
		}
	}

	return std::move(inst);
}
