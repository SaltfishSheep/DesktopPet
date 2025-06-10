#include "mouse.h"
#include <map>

#include "tick.h"
#include "config.h"

#include <iostream>

bool mouse::isPressed(mouse::MouseButton button, float judgeEnabledDelay, WindowData& data) {
	auto& state = data.states[button];
	//std::cout << button << ' ' << state.pressedTick << ' ' << tick::getTicks() << ' ' << (state.pressedTick > state.releasedTick) << ' ' << (tick::getTicks() - state.pressedTick <= judgeEnabledDelay / config::guideGlobal.tickMs) << std::endl;
	return state.pressedTick > state.releasedTick &&
		tick::getTicks() - state.pressedTick <= judgeEnabledDelay * 1000 / config::guideGlobal.tickMs;
}

bool mouse::isReleased(mouse::MouseButton button, float judgeEnabledDelay, WindowData& data) {
	auto& state = data.states[button];
	return state.releasedTick >= state.pressedTick &&
		tick::getTicks() - state.releasedTick <= judgeEnabledDelay * 1000 / config::guideGlobal.tickMs;
}

bool mouse::isClick(mouse::MouseButton button, float judgeEnabledDelay, WindowData& data) {
	auto& state = data.states[button];
	return isReleased(button) &&
		state.releasedTick - state.pressedTick <= data.longPressedBoundary / config::guideGlobal.tickMs &&
		tick::getTicks() - state.releasedTick <= judgeEnabledDelay;
}

bool mouse::isLongPressed(mouse::MouseButton button, WindowData& data) {
	auto& state = data.states[button];
	return isPressed(button, 360000) &&
		tick::getTicks() - state.pressedTick > data.longPressedBoundary * 1000 / config::guideGlobal.tickMs;
}

float mouse::getWheelState(float judgeEnabledDelay, WindowData& data) {
	return tick::getTicks() - data.wheelTick <= judgeEnabledDelay * 1000 / config::guideGlobal.tickMs ? data.wheelState : 0;
}

void mouse::handleEvent(sf::Event& event, WindowData& data) {
	if (event.is<sf::Event::MouseButtonPressed>()) {

		auto eventIn = event.getIf<sf::Event::MouseButtonPressed>();
		auto& state = data.states[(MouseButton)eventIn->button];

		if (state.pressedTick <= state.releasedTick) {
			state.pressedTick = tick::getTicks();
			//std::cout << "pressed " << buttonMap[eventIn->button] << ' ' << state.pressedTick << ' ' << state.releasedTick << std::endl;
		}
	}
	else if (event.is<sf::Event::MouseButtonReleased>()) {
		auto eventIn = event.getIf<sf::Event::MouseButtonReleased>();
		auto& state = data.states[(MouseButton)eventIn->button];
		if (state.releasedTick <= state.pressedTick)
			state.releasedTick = tick::getTicks();
	}
	else if (event.is<sf::Event::MouseWheelScrolled>()) {
		auto eventIn = event.getIf<sf::Event::MouseWheelScrolled>();
		data.wheelState = eventIn->delta;
		data.wheelTick = tick::getTicks();
	}
}