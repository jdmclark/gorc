#pragma once

#include <functional>
#include <string>
#include <stack>
#include <memory>
#include <vector>
#include <unordered_map>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>

#include "cog/vm/value.h"
#include "verb.h"
#include "exception.h"
#include "verb_binder.h"
#include "verb_id.h"

namespace Gorc {
namespace Cog {
namespace Verbs {

class VerbTable {
private:
	typedef VM::Value V;

	std::vector<std::unique_ptr<BaseVerb>> verbs;
	std::unordered_map<std::string, size_t> verb_dict;

public:
	inline VerbId GetVerb(const std::string& name) const {
		auto it = verb_dict.find(name);
		if(it == verb_dict.end()) {
			throw UndefinedVerbException();
		}

		return it->second;
	}

	inline size_t ParameterCount(VerbId id) const {
		return verbs[id.id]->ParameterCount();
	}

	inline VM::Type ReturnType(VerbId id) const {
		return verbs[id.id]->ReturnType();
	}

	inline V Invoke(VerbId id, std::stack<V>& stack) const {
		return verbs[id.id]->Invoke(stack);
	}

	inline bool IsVerbDefined(const std::string& name) const {
		auto it = verb_dict.find(name);
		return it != verb_dict.end();
	}

	template <typename ResultType, int Arity, typename T> void AddVerb(const std::string& name, T fn) {
		auto it = verb_dict.find(name);
		if(it != verb_dict.end()) {
			throw VerbRedefinitionException();
		}

		verb_dict.insert(std::make_pair(name, verbs.size()));
		verbs.push_back(std::unique_ptr<BaseVerb>(new Verb<ResultType, Arity, T>(fn)));
	}

	template <typename T> void AddVerb(const std::string& name, T fn) {
		AddVerb<typename boost::function_types::result_type<T>::type,
			boost::function_types::function_arity<T>::value, T>(name, fn);
	}
};

}
}
}
