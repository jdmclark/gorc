#pragma once

#include "framework/math/box.h"
#include "view.h"
#include <SFML/Window.hpp>

namespace gorc {
namespace game {

class view_frame {
private:
	sf::Window& Window;
	view* currentView;

public:
	view_frame(sf::Window& Window);

	inline box<2, int> get_size() const {
		return box<2, int>(make_vector(0, 0), make_vector(Window.getSize().x, Window.getSize().y));
	}

	inline void set_view(view& v) {
		currentView = &v;
	}

	inline void update(double dt) {
		if(currentView) {
			currentView->update(dt);
		}
	}

	inline void draw(double dt) {
		if(currentView) {
			currentView->draw(dt, get_size());
		}
	}
};

}
}
