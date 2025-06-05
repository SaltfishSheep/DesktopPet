#pragma once

#include "ani.h"
#include "impl_standed.hpp"
#include "file.h"
#include "asset.hpp"

using namespace ani;

namespace ani::impl{

	class ImplSinglePicture : public ImplStanded {
	public:
		int frameCount() override {
			return 1;
		}
		const sf::Texture* const frame(int index) override {
			return asset::getTexture(picPath);
		}
		void loadJson(nlohmann::json& implData) override {
			ImplStanded::loadJson(implData);
			picPath = file::getPath(std::list{ implData["picPath"].get<std::string>() });
		}
	protected:
		std::filesystem::path picPath;
	};

	class ImplSinglePictureFactory : public IAnimationImplFactory {
		const std::string implType() override {
			return "SinglePicture";
		}
		IAnimationImpl* newImpl() override {
			return new ImplSinglePicture();
		}

	};

}