#pragma once

#include "ani.h"
#include "config.h"
#include "util.h"

namespace ani {

	class ImplStanded : public IAnimationImpl {
	public:
		int frameDuration(int index) override {
			return this->frameTicks;
		}
		sf::Vector2f frameOffset(int index) override {
			return this->offset;
		}
		sf::Vector2f frameScale(int index) override {
			return this->scale;
		}
		void loadJson(nlohmann::json& implData) override {
			this->frameTicks = implData.contains("frameTicks") ? 
				implData["frameTicks"].get<int>() : config::guidePet.defaultAniTicks;

			if (implData.contains("frameOffset")) {
				nlohmann::json& frameOffset = implData["frameOffset"];
				this->offset = { frameOffset[0], frameOffset[1] };
			}

			if (implData.contains("frameScale")) {
				nlohmann::json& frameScale = implData["frameScale"];
				this->scale = { frameScale[0], frameScale[1] };
			}
		}
	protected:
		int frameTicks = 10;
		sf::Vector2f offset{ 0.0f, 0.0f };
		sf::Vector2f scale{ 1.0f, 1.0f };
	};

}
