#include "save.h"
#include "file.h"
#include "config.h"

#include "saver_default.hpp"

#include <iostream>

static std::list<save::ISaver*> savers;

void save::init()
{
	addSaver(&saverDefault);

	std::cout << "Loading save..." << std::endl;

	auto pathNew = file::getPath({ config::guideGlobal.petDirPath, "save.json" });
	auto pathOld = file::getPath({ config::guideGlobal.petDirPath, "save.json.old" });
	bool noSaveNew = !std::filesystem::exists(pathNew);
	bool noSaveOld = !std::filesystem::exists(pathOld);
	bool isLoaded = false;
	if (!noSaveNew) {
		try {
			auto json = file::loadJson(pathNew);
			for (auto saver : savers)
				saver->load(json);
		}
		catch (std::exception e) {
		}
	}
    if (!isLoaded && !noSaveOld)
	{
		try {
			auto json = file::loadJson(pathOld);
			for (auto saver : savers)
				saver->load(json);
			isLoaded = true;
		}
		catch (std::exception e) {
		}
	}
	if (noSaveNew && noSaveOld)
		std::cout << "No saving before." << std::endl;
    else if (!isLoaded)
		std::cout << "Can not load save, what's wrong?" << std::endl;
	else if (noSaveNew)
		std::cout << "Can not load lastest save, using old save." << std::endl;
}

void save::addSaver(ISaver* saver)
{
	savers.push_back(saver);
}

void save::SaverController::tick()
{
	if (tick::getTicks() - lastSaveTick >= config::guideGlobal.autoSaveTick) {
		this->save();
	}
}

void save::SaverController::save() {
	std::cout << "Saving..." << std::endl;
	lastSaveTick = tick::getTicks();
	nlohmann::json json;
	for (auto saver : savers)
		saver->save(json);
	auto savePath = file::getPath({ config::guideGlobal.petDirPath, "save.json" });
	auto savePathOld = file::getPath({ config::guideGlobal.petDirPath, "save.json.old" });
	if (std::filesystem::exists(savePath))
		std::filesystem::rename(savePath, savePathOld);
	file::saveJson(json, savePath);
    std::cout << "Saved." << std::endl;
}
