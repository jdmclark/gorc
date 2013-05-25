#include "script.h"

Gorc::Cog::Script::Script() {
	return;
}

Gorc::Cog::Instance Gorc::Cog::Script::CreateInstance() const {
	Gorc::Cog::Instance inst(*this);

	inst.Heap.resize(SymbolTable.size());

	auto it = SymbolTable.begin();
	auto jt = inst.Heap.begin();

	for( ; it != SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		(*jt) = it->DefaultValue;
	}

	return inst;
}

Gorc::Cog::Instance Gorc::Cog::Script::CreateInstance(const std::vector<VM::Value>& values) const {
	Gorc::Cog::Instance inst(*this);

	inst.Heap.resize(SymbolTable.size());

	auto it = SymbolTable.begin();
	auto jt = inst.Heap.begin();
	auto kt = values.begin();

	for( ; it != SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		if(kt != values.end() && !it->Local) {
			(*jt) = *kt;
			++kt;
		}
		else {
			(*jt) = it->DefaultValue;
		}
	}

	return inst;
}
