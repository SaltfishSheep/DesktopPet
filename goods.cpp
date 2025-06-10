#include "goods.h"
#include "asset.hpp"
#include "config.h"
#include "util.h"

void shop::init()
{
	for (const auto& entry : std::filesystem::directory_iterator(file::getPath({ "shops" }))) {
		if (!entry.is_regular_file())
			continue;
		std::string pathStr = entry.path().u8string();
		try {
			if (util::endsWith(pathStr, ".json")) {
				auto indexLast = pathStr.find_last_of('\\');
				std::string defShop = pathStr.substr(indexLast + 1, pathStr.size()-indexLast-6);
				std::cout << "Define shop: " << defShop << std::endl;
				auto json = file::loadJson(entry.path());
				auto& shop = shops[defShop];
				for (auto& item : json) {
					Good good;
					good.loadJson(item);
					shop.push_back(good);
					std::cout << "- " << good.getDisplayName() << std::endl;
				}
			}
		}
		catch (std::exception& e) {
			std::cout << "Error in loading json, file: " << pathStr << std::endl;
		}
	}
}


shop::Good::Good() :
    lang::ObjectI18N(""), price(0)
{
}

shop::Good::Good(std::string name, std::string keyI18N, unsigned int price, std::string icon) :
    lang::ObjectI18N(keyI18N),
    name(name),
    price(price),
    icon(icon)
{
}

unsigned int shop::Good::getPrice()
{
    return price;
}

sf::Texture* shop::Good::getIcon()
{
    return asset::getTexture(file::getPath({icon}));
}

void shop::Good::buy()
{
}

void shop::Good::loadJson(nlohmann::json& json)
{
    name = json["name"].get<std::string>();
    keyI18N = json["keyI18N"].get<std::string>();
    price =  json["price"].get<unsigned int>();
    icon = json["icon"].get<std::string>();
}