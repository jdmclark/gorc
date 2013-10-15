#pragma once

#include <memory>

namespace gorc {

template <typename T, typename... U> std::unique_ptr<T> make_unique(U&&... u) {
	return std::unique_ptr<T>(new T(std::forward<U>(u)...));
}

}
