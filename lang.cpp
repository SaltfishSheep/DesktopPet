#include "lang.h"

using namespace std;

void lang::load(json& jMap) {
	if (jMap.is_object())
		for (auto& item : jMap.items())
			if (item.value().is_string())
				lang::words[item.key()] = item.value();
}

string& lang::get(string& key) {
	auto iter = lang::words.find(key);
	return iter != lang::words.end() ? iter->second : key;
}