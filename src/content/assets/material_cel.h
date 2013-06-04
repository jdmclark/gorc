#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

namespace Gorc {
namespace Content {
namespace Assets {

class MaterialCel {
public:
	std::unique_ptr<sf::Image> Diffuse;
	std::unique_ptr<sf::Image> Light;

	MaterialCel() = default;
	MaterialCel(std::unique_ptr<sf::Image>& diffuse, std::unique_ptr<sf::Image>& light);
};

}
}
}
