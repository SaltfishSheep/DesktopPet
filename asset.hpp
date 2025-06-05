#pragma once

#include <map>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "file.h"

namespace asset {

	inline std::map<std::filesystem::path, sf::Texture*> texs;

	sf::Texture* getTexture(std::filesystem::path path) {
		auto iter = texs.find(path);
		if (iter != texs.end())
			return (*iter).second;
		std::cout << "Asset loaded:" << path << std::endl;
		sf::Image image = sf::Image(path);
		auto size = image.getSize();
		for (unsigned int i = 0; i < size.x; i++) {
			for (unsigned int j = 0; j < size.y; j++) {
				auto color = image.getPixel({ i, j });
				if (color.r == 0 && color.g == 0 && color.b == 0)
                    image.setPixel({ i, j }, sf::Color(1, 1, 1, color.a));
			}
		}
		sf::Texture* tex = new sf::Texture(image);
		texs[path] = tex;
		return tex;
	}

	sf::Texture* getTexture(std::list<std::string> path) {
		return getTexture(file::getPath(path));
	}

}

