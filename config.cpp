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

bool endsWith(std::string const& str, std::string const& suffix) {
	if (str.length() < suffix.length()) {
		return false;
	}
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void config::loadConfig() {
	double screenWidth = GetSystemMetrics(SM_CXSCREEN);
	double screenScale = 1;
	json cache;

	cout << "Loading config." << endl;
	try {
		guideGlobal = loadJson(cache, list{ guideGlobalPath });
		screenScale = screenWidth / guideGlobal.nomalWindowSizeX;
		guideGlobal.windowSize = {
			(int)(guideGlobal.windowSize[0] * screenScale),
			(int)(guideGlobal.windowSize[1] * screenScale)
		};
		tick::tickDuration = guideGlobal.tickMs;
	} catch (exception& e) {
		cout << "Error in loading json, file: " << guideGlobalPath << endl;
		throw exception("Basic config wrong.");
	}
	
	
	cout << "Loading languages:" << endl;
	for (const auto& entry : std::filesystem::directory_iterator(getPath({ "languages", guideGlobal.languageDirPath}))) {
		if (!entry.is_regular_file())
			continue;
		if (!endsWith(entry.path().u8string(), ".json"))
			continue;
		try {
			lang::load(loadJson(cache, entry.path()));
			cout << "- " << guideGlobal.languageDirPath << endl;
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
	

	ani::initFactories();
	trans::initFactories();

	for (const auto& entry : std::filesystem::directory_iterator(getPath({  guideGlobal.petDirPath, "anis" }))) {
		if (!entry.is_regular_file())
			continue;
		string pathStr = entry.path().u8string();
		try {
			if (endsWith(pathStr, ".anidef.json")) {
				string defPath = pathStr.substr(pathStr.find_last_of('\\') + 1);
				cout << "Define animations from " << defPath << '.' << endl;
				guidePet.loadAnimations(loadJson(cache, entry.path()));
			}
			else if (endsWith(pathStr, ".anydef.json")) {
				string defPath = pathStr.substr(pathStr.find_last_of('\\') + 1);
				cout << "Define transition anytime from " << defPath << '.' << endl;
				loadJson(cache, entry.path());
				for (auto& transitionData : cache.items()) {
					auto transition = trans::loadTransition(transitionData.value());
					ani::animationController.addTransitionAnytime(transition);
					cout << "- target: " << transition->getTransition() << endl;
				}
			}
		} catch (exception& e) {
			cout << "Error in loading json, file: " << pathStr << endl;
		}
	}

	ani::animationController.forceTo(ani::getAnimation(guidePet.defaultAnimation));

	tick::addTickUpdater(&ani::animationController);
	viewport::addRenderer(&ani::animationController);

}

void config::GuidePet::loadAnimations(nlohmann::json& animations) {
	for (auto& animationData : animations.items()) {
        auto ani = ani::loadAnimation(animationData.key(), animationData.value());
		cout << "- " << ani->getAniName() << endl;
	}
}