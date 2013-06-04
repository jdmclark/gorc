#pragma once

#include "cog/vm/type.h"
#include "cog/vm/value.h"
#include "verbbinder.h"
#include <stack>
#include <functional>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_arity.hpp>

namespace Gorc {
namespace Cog {
namespace Verbs {

template <typename T> struct helper_get_vm_type { };

template <> struct helper_get_vm_type<void> {
	static const VM::Type Type = VM::Type::Void;
};

template <> struct helper_get_vm_type<int> {
	static const VM::Type Type = VM::Type::Integer;
};

template <> struct helper_get_vm_type<float> {
	static const VM::Type Type = VM::Type::Float;
};

template <> struct helper_get_vm_type<bool> {
	static const VM::Type Type = VM::Type::Boolean;
};

template <> struct helper_get_vm_type<Math::Vector<3>> {
	static const VM::Type Type = VM::Type::Vector;
};

class BaseVerb {
private:
	const size_t parameterCount;
	const VM::Type returnType;

public:
	BaseVerb(VM::Type returnType, size_t parameterCount);
	virtual ~BaseVerb();

	inline VM::Type ReturnType() const {
		return returnType;
	}

	inline size_t ParameterCount() const {
		return parameterCount;
	}

	virtual VM::Value Invoke(std::stack<VM::Value>& stack) const = 0;
};

template <typename ResultType, int Arity, typename F> class Verb : public BaseVerb {
private:
	F functor;

public:
	Verb(F functor) : BaseVerb(helper_get_vm_type<ResultType>::Type, Arity), functor(functor) {
		return;
	}

	VM::Value Invoke(std::stack<VM::Value>& stack) const {
		VerbBinder<ResultType, Arity> binder;
		return binder.Invoke(stack, functor);
	}
};

}
}
}
