#pragma once

#include "framework/math/box.h"
#include "view.h"
#include <SFML/Window.hpp>

namespace Gorc {
namespace Game {

class ViewFrame {
private:
	sf::Window& Window;
	View* currentView;

public:
	ViewFrame(sf::Window& Window);

	inline Math::Box<2, unsigned int> GetSize() const {
		return Math::Box<2, unsigned int>(Math::Zero<2, unsigned int>(), Math::Vec(Window.GetWidth(), Window.GetHeight()));
	}

	inline void SetView(View& v) {
		currentView = &v;
	}

	inline void Update(double dt) {
		if(currentView) {
			currentView->Update(dt);
		}
	}

	inline void Draw(double dt) {
		if(currentView) {
			currentView->Draw(dt, GetSize());
		}
	}
};

}
}
