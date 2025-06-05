#pragma once

#include "ani.h"
#include "asset.hpp"
#include "impl_standed.hpp"

using namespace ani;

namespace ani::impl {

	class ImplSequentialFiles : public ImplStanded { 
		int frameCount() override {
			return picCount;
		}
		const sf::Texture* const frame(int index) override {
			return asset::getTexture(picsPath[index]);
		};
		void loadJson(nlohmann::json& implData) override {
			ImplStanded::loadJson(implData);
            picCount = implData["picCount"].get<int>();
			for (int i=1;i<=picCount;i++)
				picsPath.push_back(file::getPath({ implData["picDir"].get<std::string>(), std::to_string(i)+".png"}));
		}
	protected:
		std::vector<std::filesystem::path> picsPath;
		int picCount;
	};

	class ImplSequentialFilesFactory : public IAnimationImplFactory {
		const std::string implType() override {
			return "SequentialFiles";
		}
		IAnimationImpl* newImpl() override {
			return new ImplSequentialFiles();
		}
	};

}