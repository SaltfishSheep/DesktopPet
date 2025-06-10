#pragma once

#include <string>
#include <vector>
#include <memory>

#include "tick.h"
#include "viewport.h"
#include "lang.h"

namespace gui {

	inline const sf::Color COLOR_OVEROPTION(224, 224, 224);
	inline const sf::Color COLOR_ASBLACK(1, 1, 1);
	inline const sf::Color COLOR_DISPLAYING(0, 0, 255, 100);

	enum TextAlign {
		Left,
		Center,
		Right
	};

	bool isOver(sf::Vector2f position, sf::Vector2f size, sf::RenderWindow& reactWindow = viewport::window);

	void renderText(sf::RenderWindow& renderWindow, sf::Text text, sf::Vector2f position, TextAlign align = Center);
	void renderText(sf::RenderWindow& renderWindow, std::string text, sf::Vector2f position, TextAlign align = Center, unsigned int fontSize = 30, sf::Color fontColor = COLOR_ASBLACK);
	void renderLine(sf::RenderWindow& renderWindow, sf::Vector2f start, sf::Vector2f end, sf::Color color);
	void renderQuad(sf::RenderWindow& renderWindow, sf::Vector2f position, sf::Vector2f size, sf::Color color);
	void renderPicture(sf::RenderWindow& renderWindow, sf::Vector2f position, sf::Vector2f size, sf::Texture& texture);

	void init();

	class IGuiImplement : public virtual viewport::IRenderer, public tick::ITickUpdater {
	public:
		virtual bool isClosed() = 0;
	};

	class IGui {
	public:
		virtual bool canOpen() = 0;
		virtual IGuiImplement* open() = 0;
	};

	class GuiNode : public lang::ObjectI18N {
	public:
		GuiNode(std::string name, std::string keyI18N, IGui* gui);
		virtual ~GuiNode();
		virtual IGuiImplement* open();
	protected:
		const std::string name;
		IGui* const gui;
	};

	class GuiOption : public lang::ObjectI18N {
	public:
		GuiOption(std::string name, std::string keyI18N);
		void addSub(gui::GuiNode* node);
		GuiNode* getSub(int index);
		int getSubCount();
		virtual ~GuiOption();
	protected:
		const std::string name;
		std::vector<GuiNode*> subs;
	};

	inline GuiOption shop = { "shop", "gui.basic.shop" };
	inline GuiOption state = { "state", "gui.basic.state" };
	inline GuiOption interact = { "interact", "gui.basic.interact" };
	inline GuiOption system = { "system", "gui.basic.system" };

	inline GuiOption* options[4] = { &shop,&state,&interact,&system };

	class GuiController : public tick::ITickUpdater, public viewport::IRenderer {
	public:
		GuiController();
		void tick() override;
		void render() override;
		void addGuiImplement(IGuiImplement* renderer);
	protected:
		std::list<IGuiImplement*>  implements;
	};

	inline GuiController guiController;

}