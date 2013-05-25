#include "verb.h"

Gorc::Cog::Verbs::BaseVerb::BaseVerb(VM::Type returnType, size_t parameterCount)
	: parameterCount(parameterCount), returnType(returnType) {
	return;
}

Gorc::Cog::Verbs::BaseVerb::~BaseVerb() {
	return;
}
