#pragma once
#include <Windows.h>
#include <SFML/Graphics.hpp>

const long colorWindowBKTransparent = RGB(0, 0, 0);
const sf::Color colorSFMLBKTransparent = sf::Color(0, 0, 0);

namespace viewport{

	inline sf::RenderWindow windowParent;
	inline sf::RenderWindow window;

	class IRenderer {
	public:
		int renderLayer = 0;
		virtual void render() = 0;
		virtual bool operator==(const IRenderer&);
	};

	void initWindow(int width, int height);
	bool update();
	void addRenderer(IRenderer* renderer);

}
