#pragma once

namespace Gorc {
namespace Game {
namespace World {

class PlaceVisitor;

namespace Level { class LevelPlace; }
namespace Nothing { class NothingPlace; }

class Place {
public:
	virtual ~Place();

	virtual void Accept(PlaceVisitor& v) const = 0;
};

class PlaceVisitor {
public:
	virtual ~PlaceVisitor();

	virtual void VisitNothingPlace(const Nothing::NothingPlace& place) = 0;
	virtual void VisitLevelPlace(const Level::LevelPlace& place) = 0;
};

}
}
}
