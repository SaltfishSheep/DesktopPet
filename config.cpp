#include <json.hpp>
#include <string>
#include <iostream>

#include "file.h"
#include "config.h"
#include "lang.h"
#include "ani.h"
#include "trans.h"

using namespace config;
using namespace file;
using namespace nlohmann;
using namespace std;

const string guideGlobalPath = "config.json";

void config::loadConfig() {
	HWND hd = GetDesktopWindow();
	int zoom = GetDpiForWindow(hd);
	switch (zoom) {
	case 96:
		dpi = 1;
		break;
	case 120:
		dpi = 1.25;
		break;
	case 144:
		dpi = 1.5;
		break;
	case 192:
		dpi = 2;
		break;
	default:
		break;
	}
	screenWidth = GetSystemMetrics(SM_CXSCREEN) * dpi;
	screenHeight = GetSystemMetrics(SM_CYSCREEN) * dpi;
	std::cout << "Screen size: " << screenWidth << "x" << screenHeight << std::endl;
	double screenScale = 1;
	json cache;

	cout << "Loading config." << endl;
	try {
		guideGlobal = loadJson(cache, list{ guideGlobalPath });
		screenScale = ((float)screenWidth) / guideGlobal.nomalWindowSizeX;
		guideGlobal.windowSize = {
			(int)(guideGlobal.windowSize[0] * screenScale),
			(int)(guideGlobal.windowSize[1] * screenScale)
		};
		tick::tickDuration = guideGlobal.tickMs;
	} catch (exception& e) {
		cout << "Error in loading json, file: " << guideGlobalPath << endl;
		throw exception("Basic config wrong.");
	}

	cout << "Loading font." << endl;
	try {
		config::font = sf::Font(file::getPath({ guideGlobal.font }));
	} catch (exception& e) {
		cout << "Error in loading font, file: " << guideGlobal.font << endl;
		throw exception("Basic config wrong.");
	}
	
	
	cout << "Loading languages: " << guideGlobal.languageDirPath << endl;
	for (const auto& entry : std::filesystem::directory_iterator(getPath({ "languages", guideGlobal.languageDirPath}))) {
		if (!entry.is_regular_file())
			continue;
		if (!util::endsWith(entry.path().u8string(), ".json"))
			continue;
		try {
			lang::load(loadJson(cache, entry.path()));
			cout << "- " << entry.path().filename() << endl;
		} catch (exception& e) {
			cout << "Error in loading json, file: " << entry.path().u8string() << endl;
		}
	}
	
	cout << "Loading pet setting." << endl;
	try {
		guidePet = loadJson(cache, list<string>{ guideGlobal.petDirPath, "setting.json" });
		guidePet.defaultAniScale = {
			(float)(guidePet.defaultAniScale[0] * screenScale),
			(float)(guidePet.defaultAniScale[1] * screenScale)
		};
	} catch (exception& e) {
		cout << "Error in loading json, file: " << guideGlobal.petDirPath << "\\setting.json" << endl;
		throw exception("Basic config wrong.");
	}
	
	trans::init();
	ani::init();

	ani::animationController.forceTo(ani::getAnimation(guidePet.defaultAnimation));

}

void config::GuidePet::loadAnimations(nlohmann::json& animations) {
	for (auto& animationData : animations.items()) {
        auto ani = ani::loadAnimation(animationData.key(), animationData.value());
		cout << "- " << ani->getAniName() << endl;
	}
}