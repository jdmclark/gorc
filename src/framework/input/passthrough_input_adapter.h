#pragma once

#include "input_adapter.h"
#include <array>
#include <type_traits>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace gorc {
namespace input {

template <typename T> auto dispatch_set_mouse_cursor_position(T& view, const time& time, const vector<2, int>& cursor_pos)
	-> decltype(view.set_mouse_cursor_position(time, cursor_pos)) {
	view.set_mouse_cursor_position(time, cursor_pos);
}

template <typename... T> void dispatch_set_mouse_cursor_position(const T&... args) {
	return;
}

template <typename T> auto dispatch_wants_mouse_focus(const T& view, int) -> decltype(view.wants_mouse_focus()) {
	return view.wants_mouse_focus();
}

template <typename T> bool dispatch_wants_mouse_focus(const T& view, ...) {
	return false;
}

template <typename T> auto dispatch_hide_mouse_cursor(const T& view, int) -> decltype(view.wants_mouse_focus()) {
	return view.hide_mouse_cursor();
}

template <typename T> bool dispatch_hide_mouse_cursor(const T& view, ...) {
	return false;
}

template <typename T> auto dispatch_wants_keyboard_focus(const T& view, int) -> decltype(view.wants_keyboard_focus()) {
	return view.wants_keyboard_focus();
}

template <typename T> bool dispatch_wants_keyboard_focus(const T& view, ...) {
	return false;
}

template <typename T> auto dispatch_handle_mouse_input(T& view, const time& time) -> decltype(view.handle_mouse_input(time)) {
	view.handle_mouse_input(time);
}

template <typename... T> void dispatch_handle_mouse_input(const T&... args) {
	return;
}

template <typename T> auto dispatch_handle_keyboard_input(T& view, const time& time) -> decltype(view.handle_keyboard_input(time)) {
	view.handle_keyboard_input(time);
}

template <typename... T> void dispatch_handle_keyboard_input(const T&... args) {
	return;
}

template <typename T> auto dispatch_handle_text_input(T& view, const time& time, char ch) -> decltype(view.handle_text_input(time, ch)) {
	view.handle_text_input(time, ch);
}

template <typename... T> void dispatch_handle_text_input(const T&... args) {
	return;
}

template <typename ViewT> class passthrough_input_adapter : public input_adapter {
private:
	ViewT& view;

public:
	passthrough_input_adapter(ViewT& view) : view(view) {
		return;
	}

	virtual void set_mouse_cursor_position(const time& time, const vector<2, int>& cursor_pos) override {
		dispatch_set_mouse_cursor_position(view, time, cursor_pos);
	}

	virtual bool wants_mouse_focus() const override {
		return dispatch_wants_mouse_focus(view, 0);
	}

	virtual bool hide_mouse_cursor() const override {
		return dispatch_hide_mouse_cursor(view, 0);
	}

	virtual bool wants_keyboard_focus() const override {
		return dispatch_wants_keyboard_focus(view, 0);
	}

	virtual void handle_mouse_input(const time& time) override {
		dispatch_handle_mouse_input(view, time);
	}

	virtual void handle_keyboard_input(const time& time) override {
		dispatch_handle_keyboard_input(view, time);
	}

	virtual void handle_text_input(const time& time, char ch) override {
		dispatch_handle_text_input(view, time, ch);
	}
};

}
}
