#pragma once

namespace Gorc {
namespace Game {
namespace Screen {

class PlaceVisitor;

namespace Action { class ActionPlace; }

class Place {
public:
	virtual ~Place();

	virtual void Accept(PlaceVisitor& v) const = 0;
};

class PlaceVisitor {
public:
	virtual ~PlaceVisitor();

	virtual void VisitActionPlace(const Action::ActionPlace& place) = 0;
};

}
}
}
