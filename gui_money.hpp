#pragma once
#include "util.h"
#include "config.h"
#include "gui.h"
#include "saver_default.hpp"

static std::string money_fall_keyI18N = "gui.basic.interact.money.fall";

static class GuiMoneyImplement : public gui::IGuiImplement {
public:
	GuiMoneyImplement() {
		renderLayer = 1;
	}
	double duration = 1;
	std::list<long long> triggers;
	void render() override {
		// ÓÃiterator±éÀútriggers
		for (auto it = triggers.begin(); it != triggers.end();) {
            long long trigger = *it;
			double passedTime = (tick::getTicks() + tick::getPartialTick() - trigger) * config::guideGlobal.tickMs / 1000.0;
            if (passedTime >= duration) 
				it = this->triggers.erase(it);
			else {
				sf::Text text{config::font, util::convert(lang::get(money_fall_keyI18N))};
				text.setFillColor(sf::Color::Yellow);
				auto& windowSize = config::guideGlobal.windowSize;
				auto position = sf::Vector2f(windowSize[0]/2, passedTime/duration * windowSize[1]);
				gui::renderText(viewport::window, text, position);
				it++;
			}
        }
	}
	void tick() override{}
	bool isClosed() override {
		return triggers.size() <= 0;
	}
};

namespace gui { 
	class GuiMoney : public gui::IGui {
	public:
		bool canOpen() override {
			return true;
		}
		IGuiImplement* open() override {
			renderer.triggers.push_back(tick::getTicks());
			save::saverDefault.addMoney(1000);
			return &renderer;
		}
	protected:
		GuiMoneyImplement renderer;
	};
}
