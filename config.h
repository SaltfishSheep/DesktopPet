#pragma once

#include <json.hpp>
#include <SFML/Graphics.hpp>


namespace config {

	inline int screenWidth, screenHeight;

	inline float dpi;

	inline sf::Font font;

	class GuideGlobal {
	public:
		double tickMs = 20;
		long long autoSaveTick = 500;
		std::string languageDirPath;
		std::string petDirPath;
		std::vector<int> windowSize = { 256, 256 };
		int nomalWindowSizeX = 1920;
		std::string font;
		std::string shopIcon;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(GuideGlobal, tickMs, autoSaveTick, languageDirPath, petDirPath, windowSize, nomalWindowSizeX, font, shopIcon);
	};

	inline GuideGlobal guideGlobal;

	class GuidePet {
	public:

		int defaultAniTicks = 10;
		std::vector<float> defaultAniOffset = { 0, 0 };
		std::vector<float> defaultAniScale = { 1, 1 };
		std::string defaultAnimation = "default";

		int walkSpeed = 20;
		int walkStepMax = 20;

		void loadAnimations(nlohmann::json& animations);

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(GuidePet, 
			defaultAniTicks, defaultAniOffset, defaultAniScale, defaultAnimation, 
			walkSpeed, walkStepMax);
	};

	inline GuidePet guidePet;

	void loadConfig();
}