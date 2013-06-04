#include "material_cel.h"

Gorc::Content::Assets::MaterialCel::MaterialCel(std::unique_ptr<sf::Image>& diffuse, std::unique_ptr<sf::Image>& light)
	: Diffuse(std::move(diffuse)), Light(std::move(light)) {
	return;
}
