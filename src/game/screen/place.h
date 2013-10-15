#pragma once

namespace gorc {
namespace game {
namespace screen {

class place_visitor;

namespace action { class action_place; }

class place {
public:
	virtual ~place();

	virtual void accept(place_visitor& v) const = 0;
};

class place_visitor {
public:
	virtual ~place_visitor();

	virtual void visit_action_place(const action::action_place& place) = 0;
};

}
}
}
