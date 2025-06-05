#pragma once

#include <json.hpp>


namespace config {

	class GuideGlobal {
	public:
		double tickMs = 20;
		std::string languageDirPath;
		std::string petDirPath;
		std::vector<int> windowSize = { 256, 256 };
		int nomalWindowSizeX = 1920;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(GuideGlobal, tickMs, languageDirPath, petDirPath, windowSize, nomalWindowSizeX);
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