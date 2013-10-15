#pragma once

#include "presenter.h"
#include <memory>

namespace gorc {
namespace place {

template <typename T> class presenter_mapper {
public:
	virtual ~presenter_mapper() {
		return;
	}

	virtual std::unique_ptr<presenter> get_presenter(const T& place) = 0;
};

}
}
