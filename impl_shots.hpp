#pragma once

#include "asset.hpp"
#include "ani.h"
#include "impl_standed.hpp"
#include "file.h"

using namespace ani;

namespace ani::impl {


	class ImplSequentialShots : public ImplStanded {
	public:
		int frameCount() override {
			return xCount * yCount;
		}
		sf::IntRect* frameRect(int index) override { 
			auto xIndex = index % xCount;
			auto yIndex = index / xCount;
			if (yIndex >= yCount) return nullptr;
			auto startPoint = picStartPoint + sf::Vector2i(xIndex * shotSize.x, yIndex * shotSize.y);
            rectCache = sf::IntRect(startPoint, shotSize);
			return &rectCache; 
		}
		const sf::Texture* const frame(int index) override {
			return asset::getTexture(picPath);
		};
		void loadJson(nlohmann::json& implData) override {
			ImplStanded::loadJson(implData);
			picPath = file::getPath(std::list{ implData["picPath"].get<std::string>() });
			if (implData.contains("xCount")) xCount = implData["xCount"];
			if (implData.contains("yCount")) yCount = implData["yCount"];
			if (implData.contains("picStartPoint")) {
				auto picStartPointJson = implData["picStartPoint"].get<std::vector<int>>();
				picStartPoint = {picStartPointJson[0], picStartPointJson[1]};
			}
			if (implData.contains("shotSize")) {
				auto shotSizeJson = implData["shotSize"].get<std::vector<int>>();
				shotSize = {shotSizeJson[0], shotSizeJson[1]};
			}
		}
	protected:
		std::filesystem::path picPath;
		int xCount = 1, yCount = 1;
		sf::Vector2i picStartPoint{0,0};
		sf::Vector2i shotSize{ 256,256 };

		sf::IntRect rectCache;
	};

	class ImplSequentialShotsFactory : public IAnimationImplFactory {
		const std::string implType() override {
			return "SequentialShots";
		}
		IAnimationImpl* newImpl() override {
			return new ImplSequentialShots();
		}
	};

}