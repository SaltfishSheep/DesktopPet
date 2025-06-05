#pragma once

#include "ani.h"

using namespace ani;

namespace ani::impl {

	struct IndexRelative {
		IAnimationImpl* impl;
		int indexRelative;
	};

	class ImplCustom :public IAnimationImpl {
	public:
		int frameCount() override {
			int count = 0;
			for (auto impl : impls)
                count += impl->frameCount();
			return count;
		}
		int frameDuration(int index) override {
			auto indexRelative = getIndexRelative(index);
			return indexRelative.impl->frameDuration(indexRelative.indexRelative);
		}
		sf::Vector2f frameOffset(int index) override {
			auto indexRelative = getIndexRelative(index);
            return indexRelative.impl->frameOffset(indexRelative.indexRelative);
		}
		sf::Vector2f frameScale(int index) override {
			auto indexRelative = getIndexRelative(index);
            return indexRelative.impl->frameScale(indexRelative.indexRelative);
		}
		sf::IntRect* frameRect(int index) override {
			auto indexRelative = getIndexRelative(index);
            return indexRelative.impl->frameRect(indexRelative.indexRelative);
		}
		const sf::Texture* const frame(int index) override {
			auto indexRelative = getIndexRelative(index);
            return indexRelative.impl->frame(indexRelative.indexRelative);
		}
		void loadJson(nlohmann::json& implData) override {
			auto& implsJson = implData["impls"];
            for (auto& implJson : implsJson) 
                impls.push_back(ani::getAnimationImpl(implJson));
		}
	protected:
		std::list<IAnimationImpl*> impls;
		virtual IndexRelative getIndexRelative(int index) {
			int totalCount = 0;
            for (auto impl : impls) {
                int count = impl->frameCount();
                if (index < totalCount + count) {
                    return { impl, index - totalCount };
                }
                totalCount += count;
            }
			return { nullptr, 0 };
		}
	};

	class ImplCustomFactory : public IAnimationImplFactory {
		const std::string implType() override {
			return "Custom";
		}
		IAnimationImpl* newImpl() override {
			return new ImplCustom();
		}
	};

}