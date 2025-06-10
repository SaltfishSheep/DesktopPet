#pragma once
#include <map>
#include <string>
#include <json.hpp>
#include <list>

using namespace nlohmann;

namespace lang {

	class ObjectI18N {
	public :
		ObjectI18N(std::string key);
        virtual std::string getDisplayName();
	protected:
		std::string keyI18N;
	};

	inline std::map<std::string, std::string> words;

	std::string get(std::string key);

	void load(json& jsonMap);

}