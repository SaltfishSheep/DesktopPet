#pragma once

#include "ani.h"

using namespace ani;

namespace ani::impl {

	class ImplRepeat : public IAnimationImpl {
	public:
		int frameCount() override {
			return getRepeatAni()->getImpl()->frameCount();
		}
		int frameDuration(int index) override {
			return getRepeatAni()->getImpl()->frameDuration(index);
		}
		sf::Vector2f frameOffset(int index) override {
			return getRepeatAni()->getImpl()->frameOffset(index);
		}
		sf::Vector2f frameScale(int index) override {
			return getRepeatAni()->getImpl()->frameScale(index);
		}
		sf::IntRect* frameRect(int index) override { 
			return getRepeatAni()->getImpl()->frameRect(index);
		}
		const sf::Texture* const frame(int index) override {
			return getRepeatAni()->getImpl()->frame(index);
		}
		void loadJson(nlohmann::json& implData) override {
			repeatAni = implData["repeatAni"];
		}
	protected:
		virtual Animation* getRepeatAni() {
			if (aniLastLoaded != ani::getAnimationLastLoaded()) {
                aniLastLoaded = ani::getAnimationLastLoaded();
                repeatAniCache = ani::getAnimation(repeatAni);
			}
			return repeatAniCache;
		}
	private:
		std::string repeatAni;
		ani::Animation* repeatAniCache;
		ani::Animation* aniLastLoaded;
	};

	class ImplRepeatFactory : public IAnimationImplFactory {
		const std::string implType() override {
			return "Repeat";
		}
		IAnimationImpl* newImpl() override {
			return new ImplRepeat();
		}
	};

}