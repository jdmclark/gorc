#include "verb.h"

gorc::cog::verbs::base_verb::base_verb(vm::type returnType, size_t parameterCount)
    : parameterCount(parameterCount), returnType(returnType) {
    return;
}

gorc::cog::verbs::base_verb::~base_verb() {
    return;
}
