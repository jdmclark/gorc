#pragma once

#include "presenter.h"
#include <memory>

namespace Gorc {
namespace Place {

template <typename T> class PresenterMapper {
public:
	virtual ~PresenterMapper() {
		return;
	}

	virtual std::unique_ptr<Presenter> GetPresenter(const T& place) = 0;
};

}
}
