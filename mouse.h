#pragma once

#include <SFML/Graphics.hpp>
#include <map>

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

	struct WindowData {
		float longPressedBoundary = 0.2;

		std::map<mouse::MouseButton, mouse::MouseState> states;

		float wheelState = 0;
		long wheelTick = 0;
	};

	inline WindowData main;

	bool isPressed(mouse::MouseButton button, float judgeEnabledDelay = 0, WindowData& data = main);
	bool isReleased(mouse::MouseButton button, float judgeEnabledDelay = 0, WindowData& data = main);
	bool isClick(mouse::MouseButton button, float judgeEnabledDelay = 0, WindowData& data = main);
	bool isLongPressed(mouse::MouseButton button, WindowData& data = main);
	float getWheelState(float judgeEnabledDelay = 0, WindowData& data = main);
	void handleEvent(sf::Event& event, WindowData& data = main);

}
