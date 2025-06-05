#pragma once
#include <map>
#include <string>
#include <json.hpp>
#include <list>

using namespace nlohmann;

namespace lang {
	inline std::map<std::string, std::string> words;

	std::string& get(std::string& key);

	void load(json& jsonMap);

}