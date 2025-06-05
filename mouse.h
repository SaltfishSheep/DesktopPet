#pragma once

#include <SFML/Graphics.hpp>

namespace mouse {

	enum MouseButton {
		Left = sf::Mouse::Button::Left,
		Right = sf::Mouse::Button::Right,
		Middle = sf::Mouse::Button::Middle,
		Extra1 = sf::Mouse::Button::Extra1,
		Extra2 = sf::Mouse::Button::Extra2
	};

	struct MouseState {
		long pressedTick = 0, releasedTick = 0;
	};

	bool isPressed(mouse::MouseButton button, float judgeEnabledDelay = 0);
	bool isReleased(mouse::MouseButton button, float judgeEnabledDelay = 0);
	bool isClick(mouse::MouseButton button, float judgeEnabledDelay = 0);
	bool isLongPressed(mouse::MouseButton button);
	float getWheelState(float judgeEnabledDelay = 0);
	void handleEvent(sf::Event& event);

}
