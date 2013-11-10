#pragma once

#include "place.h"
#include "framework/place/presenter_mapper.h"
#include <memory>

namespace gorc {
namespace game {

class components;

namespace world {

class presenter_mapper : public gorc::place::presenter_mapper<place> {
private:
	class presenter_mapper_place_visitor : public place_visitor {
	private:
		components& components;

	public:
		std::unique_ptr<gorc::place::presenter> presenter;

		presenter_mapper_place_visitor(class components& components);

		void visit_nothing_place(const nothing::nothing_place& place);
		void visit_level_place(const level::level_place& place);
	};

	components* components;

public:
	inline void set_components(class components& cmp) {
		components = &cmp;
	}

	std::unique_ptr<gorc::place::presenter> get_presenter(const place& place);
};

}
}
}
