#pragma once

#include "node.h"
#include "framework/utility/make_unique.h"
#include <memory>

namespace gorc {
namespace cog {
namespace ast {

class factory {
private:
	std::vector<std::unique_ptr<node>> nodes;

	template <typename T> class value_storage_node : public node {
	public:
		T value;

		value_storage_node() {
			return;
		}

		explicit value_storage_node(const T& value) : value(value) {
			return;
		}

		void accept(visitor& v) {
			return;
		}
	};

public:
	template <typename T> T* store_value(const T& value) {
		auto val = make_unique<value_storage_node<T>>(value);
		auto* val_ptr = val.get();
		nodes.push_back(std::move(val));
		return &val_ptr->value;
	}

	template <typename T> std::vector<T*>* make_list() {
		auto val = make_unique<value_storage_node<std::vector<T*>>>();
		auto* val_ptr = val.get();
		nodes.push_back(std::move(val));
		return &val_ptr->value;
	}

	template <typename T> std::vector<T*>* make_list(T* initial_value) {
		auto* val_ptr = make_list<T>();
		val_ptr->push_back(initial_value);
		return val_ptr;
	}

	template <typename T, typename... V> T* make(const text::location& yyl, V&&... args) {
		auto val = make_unique<T>(std::forward<V>(args)...);
		val->location = yyl;
		auto* val_ptr = val.get();
		nodes.push_back(std::move(val));
		return val_ptr;
	}
};
}
}
}
