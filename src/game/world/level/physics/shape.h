#pragma once

#include <tuple>
#include "framework/math/vector.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Physics {

using Segment = std::tuple<Math::Vector<3>, Math::Vector<3>>;
using Sphere = std::tuple<Math::Vector<3>, float>;
using SweptSphere = std::tuple<Segment, float>;

}
}
}
}
}
