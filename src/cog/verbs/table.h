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

namespace gorc {
namespace cog {
namespace verbs {

class verb_table {
private:
	typedef vm::value V;

	std::vector<std::unique_ptr<base_verb>> verbs;
	std::unordered_map<std::string, size_t> verb_dict;

public:
	inline verb_id get_verb(const std::string& name) const {
		auto it = verb_dict.find(name);
		if(it == verb_dict.end()) {
			throw undefined_verb_exception();
		}

		return it->second;
	}

	inline size_t parameter_count(verb_id id) const {
		return verbs[id.id]->parameter_count();
	}

	inline vm::type return_type(verb_id id) const {
		return verbs[id.id]->return_type();
	}

	inline V invoke(verb_id id, std::stack<V>& stack) const {
		return verbs[id.id]->invoke(stack);
	}

	inline bool is_verb_defined(const std::string& name) const {
		auto it = verb_dict.find(name);
		return it != verb_dict.end();
	}

	template <typename ResultType, int Arity, typename T> void add_verb(const std::string& name, T fn) {
		auto it = verb_dict.find(name);
		if(it != verb_dict.end()) {
			throw verb_redefinition_exception();
		}

		verb_dict.insert(std::make_pair(name, verbs.size()));
		verbs.push_back(std::unique_ptr<base_verb>(new verb<ResultType, Arity, T>(fn)));
	}

	template <typename T> void add_verb(const std::string& name, T fn) {
		add_verb<typename boost::function_types::result_type<T>::type,
			boost::function_types::function_arity<T>::value, T>(name, fn);
	}
};

}
}
}
