#pragma once

/* TODO:
 * Rewrite this code when C++11 support in Visual C++ is better.
 */

#include "cog/vm/value.h"
#include <stack>

namespace gorc {
namespace cog {
namespace verbs {

template <typename T, int n> class verb_binder;

#define GORC_CV_BINDER_ARG(n) vm::value v##n = stack.top(); stack.pop();

#define GORC_CV_BINDER_ARGS_0
#define GORC_CV_BINDER_ARGS_1 GORC_CV_BINDER_ARG(1); GORC_CV_BINDER_ARGS_0
#define GORC_CV_BINDER_ARGS_2 GORC_CV_BINDER_ARG(2); GORC_CV_BINDER_ARGS_1
#define GORC_CV_BINDER_ARGS_3 GORC_CV_BINDER_ARG(3); GORC_CV_BINDER_ARGS_2
#define GORC_CV_BINDER_ARGS_4 GORC_CV_BINDER_ARG(4); GORC_CV_BINDER_ARGS_3
#define GORC_CV_BINDER_ARGS_5 GORC_CV_BINDER_ARG(5); GORC_CV_BINDER_ARGS_4
#define GORC_CV_BINDER_ARGS_6 GORC_CV_BINDER_ARG(6); GORC_CV_BINDER_ARGS_5
#define GORC_CV_BINDER_ARGS_7 GORC_CV_BINDER_ARG(7); GORC_CV_BINDER_ARGS_6
#define GORC_CV_BINDER_ARGS_8 GORC_CV_BINDER_ARG(8); GORC_CV_BINDER_ARGS_7
#define GORC_CV_BINDER_ARGS_9 GORC_CV_BINDER_ARG(9); GORC_CV_BINDER_ARGS_8
#define GORC_CV_BINDER_ARGS_10 GORC_CV_BINDER_ARG(10); GORC_CV_BINDER_ARGS_9

#define GORC_CV_BINDER_ARG_PASS_0
#define GORC_CV_BINDER_ARG_PASS_1 v1
#define GORC_CV_BINDER_ARG_PASS_2 GORC_CV_BINDER_ARG_PASS_1, v2
#define GORC_CV_BINDER_ARG_PASS_3 GORC_CV_BINDER_ARG_PASS_2, v3
#define GORC_CV_BINDER_ARG_PASS_4 GORC_CV_BINDER_ARG_PASS_3, v4
#define GORC_CV_BINDER_ARG_PASS_5 GORC_CV_BINDER_ARG_PASS_4, v5
#define GORC_CV_BINDER_ARG_PASS_6 GORC_CV_BINDER_ARG_PASS_5, v6
#define GORC_CV_BINDER_ARG_PASS_7 GORC_CV_BINDER_ARG_PASS_6, v7
#define GORC_CV_BINDER_ARG_PASS_8 GORC_CV_BINDER_ARG_PASS_7, v8
#define GORC_CV_BINDER_ARG_PASS_9 GORC_CV_BINDER_ARG_PASS_8, v9
#define GORC_CV_BINDER_ARG_PASS_10 GORC_CV_BINDER_ARG_PASS_9, v10

#define GORC_CV_BINDER(n)																						\
template <typename T> class verb_binder<T, n>																	\
{																												\
public:																											\
	template <typename U> vm::value invoke(std::stack<vm::value>& stack, U fn)									\
	{																											\
		GORC_CV_BINDER_ARGS_##n;																				\
		return vm::value(fn(GORC_CV_BINDER_ARG_PASS_##n));															\
	}																											\
};																												\
																												\
template <> class verb_binder<void, n>																			\
{																												\
public:																											\
	template <typename U> vm::value invoke(std::stack<vm::value>& stack, U fn)									\
	{																											\
		GORC_CV_BINDER_ARGS_##n;																				\
		fn(GORC_CV_BINDER_ARG_PASS_##n);																		\
		return vm::value();																						\
	}																											\
};

GORC_CV_BINDER(0);
GORC_CV_BINDER(1);
GORC_CV_BINDER(2);
GORC_CV_BINDER(3);
GORC_CV_BINDER(4);
GORC_CV_BINDER(5);
GORC_CV_BINDER(6);
GORC_CV_BINDER(7);
GORC_CV_BINDER(8);
GORC_CV_BINDER(9);
GORC_CV_BINDER(10);

}
}
}
