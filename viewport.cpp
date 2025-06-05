#include "viewport.h"

#include "file.h"
#include "config.h"

#include <list>

using namespace viewport;
using namespace sf;

std::list<IRenderer*> renderers;

void viewport::initWindow(int width, int height) {
    //window = RenderWindow(sf::VideoMode(sf::Vector2u{ (unsigned int)config::guideGlobal.windowSize[0], (unsigned int)config::guideGlobal.windowSize[1] }), "DesktopPet", sf::Style::Default);

    window = RenderWindow(sf::VideoMode(sf::Vector2u{ (unsigned int)config::guideGlobal.windowSize[0], (unsigned int)config::guideGlobal.windowSize[1] }), "DesktopPet", sf::Style::None);
    HWND handle = window.getNativeHandle();
    SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(handle, colorWindowBKTransparent, 0, LWA_COLORKEY);
    SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    window.clear(colorSFMLBKTransparent);
}

bool viewport::update() {
    if (window.isOpen()) {
        window.clear(colorSFMLBKTransparent);

        for (IRenderer* renderer : renderers)
            renderer->render();

        window.display();
        return true;
    }
    return false;
}

void viewport::addRenderer(IRenderer* renderer) {
    std::list<IRenderer*> tempList;
    while (!renderers.empty()) {
        IRenderer* endOne = renderers.back();
        if (endOne->renderLayer <= renderer->renderLayer)
            break;
        renderers.pop_back();
        tempList.push_back(endOne);
    }
    renderers.push_back(renderer);
    while (!tempList.empty()) {
        IRenderer* endOne = tempList.back();
        tempList.pop_back();
        renderers.push_back(endOne);
    }
}