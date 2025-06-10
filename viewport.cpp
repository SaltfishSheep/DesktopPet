#include "viewport.h"

#include "file.h"
#include "config.h"

#include <list>
#include <iostream>

using namespace viewport;
using namespace sf;

std::list<IRenderer*> renderers;

WNDPROC g_originalWndProc;
// 在自定义窗口过程中强制响应透明区域
LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_NCHITTEST) {
        // 透明区域也视为可点击区域
        LRESULT hit = DefWindowProc(hWnd, uMsg, wParam, lParam);
        return (hit == HTTRANSPARENT) ? HTCLIENT : hit;
    }
    return CallWindowProc(g_originalWndProc, hWnd, uMsg, wParam, lParam);
}

void viewport::initWindow(int width, int height) {
    //window = RenderWindow(sf::VideoMode(sf::Vector2u{ (unsigned int)config::guideGlobal.windowSize[0], (unsigned int)config::guideGlobal.windowSize[1] }), "DesktopPet", sf::Style::Default);

    window = RenderWindow(sf::VideoMode(sf::Vector2u{ (unsigned int)(config::guideGlobal.windowSize[0] * 1.2), (unsigned int)config::guideGlobal.windowSize[1] }), "DesktopPet", sf::Style::None);
    HWND handle = window.getNativeHandle();

    g_originalWndProc = (WNDPROC)GetWindowLongPtr(handle, GWLP_WNDPROC);
    SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)&CustomWndProc);
    SetWindowLong(handle, GWL_EXSTYLE, (GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOOLWINDOW));
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
    for (auto rendererIn : renderers)
        if ((*rendererIn) == (*renderer))
            return;
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

bool viewport::IRenderer::operator==(const IRenderer& renderer)
{
    return (IRenderer*)this == (&renderer) ;
}
