#pragma once

#include <filesystem>
#include <fstream>
#include <json.hpp>
//#include <iostream>

namespace file {
	std::filesystem::path getPath(std::list<std::string> pathRelative);
	std::ifstream getInput(std::filesystem::path path, bool isBinary);
	std::ifstream getInput(std::list<std::string> pathRelative, bool isBinary);
	std::ofstream getOutput(std::filesystem::path path, bool isBinary);
	std::ofstream getOutput(std::list<std::string> pathRelative, bool isBinary);
	nlohmann::json& loadJson(nlohmann::json& store, std::filesystem::path path);
	nlohmann::json& loadJson(nlohmann::json& store, std::list<std::string> pathRelative);
	inline nlohmann::json loadJson(std::filesystem::path path) {
		nlohmann::json store;
		loadJson(store, path);
		return store;
	}
	inline nlohmann::json loadJson(std::list<std::string> pathRelative) {
		return loadJson(getPath(pathRelative));
	}
	void saveJson(nlohmann::json& data, std::filesystem::path path);
	void saveJson(nlohmann::json& data, std::list<std::string> pathRelative);
}