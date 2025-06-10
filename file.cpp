#pragma warning(disable : 4996)
#include "file.h"
#include <filesystem>

#include <sstream>
#include <fstream>
#include <iostream>
#include <json.hpp>


using namespace file;
using namespace std;

namespace fs = std::filesystem;

fs::path dir;
std::string dirStr;
bool isInited = false;

void initFileHandler() {
	if (isInited)
		return;
	dir = fs::current_path();
	dirStr = dir.u8string();
	isInited = true;
}

fs::path file::getPath(std::list<std::string> pathRelative) {
	initFileHandler();
	std::stringstream ss;
	ss << dirStr << "\\" << "Assets";
	for (auto& pathNode : pathRelative)
		ss << "\\" << pathNode;
	return fs::u8path(ss.str());
}

std::ifstream file::getInput(std::filesystem::path path, bool isBinary) {
	initFileHandler();
	std::ifstream ifs;
	ifs.open(path, std::ios::in | (isBinary ? ios::binary : 0));
	return ifs;
}

std::ifstream file::getInput(std::list<std::string> pathRelative, bool isBinary) {
	return getInput(getPath(pathRelative), isBinary);
}

std::ofstream file::getOutput(std::filesystem::path path, bool isBinary) {
	initFileHandler();
	std::ofstream ofs;
	ofs.open(path, std::ios::out | (isBinary ? ios::binary : 0));
	return ofs;
}

std::ofstream file::getOutput(std::list<std::string> pathRelative, bool isBinary) {
	return getOutput(getPath(pathRelative), isBinary);
}

nlohmann::json& file::loadJson(nlohmann::json& store, std::filesystem::path path) {
	auto input = getInput(path, false);
	try {
		store = nlohmann::json::parse(input);
	}
	catch (exception e) {
		throw exception("Error on loading json file. Maybe it is not in the correct format");
	}
    input.close();
	return store;
}

nlohmann::json& file::loadJson(nlohmann::json& store, std::list<std::string> pathRelative) {
	return loadJson(store, getPath(pathRelative));
}

void file::saveJson(nlohmann::json& data, std::filesystem::path path) {
	auto output = getOutput(path, false);
	output << std::setw(4) << data;
	output.close();
}

void file::saveJson(nlohmann::json& data, std::list<std::string> pathRelative) {
	saveJson(data, getPath(pathRelative));
}


