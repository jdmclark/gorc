#pragma once

namespace gorc {
namespace game {
namespace world {

class place_visitor;

namespace level { class level_place; }
namespace nothing { class nothing_place; }

class place {
public:
	virtual ~place();

	virtual void accept(place_visitor& v) const = 0;
};

class place_visitor {
public:
	virtual ~place_visitor();

	virtual void visit_nothing_place(const nothing::nothing_place& place) = 0;
	virtual void visit_level_place(const level::level_place& place) = 0;
};

}
}
}
