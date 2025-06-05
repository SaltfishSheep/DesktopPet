#include "mouse.h"
#include <map>

#include "tick.h"
#include "config.h"

#include <iostream>

float longPressedBoundary = 0.2;

std::map<mouse::MouseButton, mouse::MouseState> states;

float wheelState = 0;
long wheelTick = 0;

bool mouse::isPressed(mouse::MouseButton button, float judgeEnabledDelay) {
	auto& state = states[button];
	//std::cout << button << ' ' << state.pressedTick << ' ' << tick::getTicks() << ' ' << (state.pressedTick > state.releasedTick) << ' ' << (tick::getTicks() - state.pressedTick <= judgeEnabledDelay / config::guideGlobal.tickMs) << std::endl;
	return state.pressedTick > state.releasedTick &&
		tick::getTicks() - state.pressedTick <= judgeEnabledDelay * 1000 / config::guideGlobal.tickMs;
}

bool mouse::isReleased(mouse::MouseButton button, float judgeEnabledDelay) {
	auto& state = states[button];
	return state.releasedTick >= state.pressedTick &&
		tick::getTicks() - state.releasedTick <= judgeEnabledDelay * 1000 / config::guideGlobal.tickMs;
}

bool mouse::isClick(mouse::MouseButton button, float judgeEnabledDelay) {
	auto& state = states[button];
	return isReleased(button) &&
		state.releasedTick - state.pressedTick <= longPressedBoundary / config::guideGlobal.tickMs &&
		tick::getTicks() - state.releasedTick <= judgeEnabledDelay;
}

bool mouse::isLongPressed(mouse::MouseButton button) {
	auto& state = states[button];
	return isPressed(button, 360000) &&
		tick::getTicks() - state.pressedTick > longPressedBoundary * 1000 / config::guideGlobal.tickMs;
}

float mouse::getWheelState(float judgeEnabledDelay) {
	return tick::getTicks() - wheelTick <= judgeEnabledDelay * 1000 / config::guideGlobal.tickMs ? wheelState : 0;
}

void mouse::handleEvent(sf::Event& event) {
	if (event.is<sf::Event::MouseButtonPressed>()) {

		auto eventIn = event.getIf<sf::Event::MouseButtonPressed>();
		auto& state = states[(MouseButton)eventIn->button];

		if (state.pressedTick <= state.releasedTick) {
			state.pressedTick = tick::getTicks();
			//std::cout << "pressed " << buttonMap[eventIn->button] << ' ' << state.pressedTick << ' ' << state.releasedTick << std::endl;
		}
	}
	else if (event.is<sf::Event::MouseButtonReleased>()) {
		auto eventIn = event.getIf<sf::Event::MouseButtonReleased>();
		auto& state = states[(MouseButton)eventIn->button];
		if (state.releasedTick <= state.pressedTick)
			state.releasedTick = tick::getTicks();
	}
	else if (event.is<sf::Event::MouseWheelScrolled>()) {
		auto eventIn = event.getIf<sf::Event::MouseWheelScrolled>();
		wheelState = eventIn->delta;
		wheelTick = tick::getTicks();
	}
}