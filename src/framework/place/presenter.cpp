#include "presenter.h"

Gorc::Place::Presenter::~Presenter() {
	return;
}

bool Gorc::Place::Presenter::CanStop() {
	return true;
}

void Gorc::Place::Presenter::Stop() {
	return;
}
