#pragma once
#include "lang.h"

#include <map>
#include <list>
#include <SFML/Graphics.hpp>
#include <json.hpp>

namespace shop {

	void init();

	class Good : public lang::ObjectI18N {
	public:
		Good();
		Good(std::string name, std::string keyI18N, unsigned int price, std::string icon);
		virtual unsigned int getPrice();
		virtual sf::Texture* getIcon();
		virtual void buy();
		virtual	void loadJson(nlohmann::json& json);
	protected:
		std::string name;
		unsigned int price;
		std::string icon;
	};

	inline std::map<std::string, std::list<Good>> shops;

}
