#include "gui_shop_default.h"
#include "util.h"
#include "mouse.h"
#include "asset.hpp"
#include "config.h"
#include "goods.h"
#include "saver_default.hpp"

static mouse::WindowData shopDefaultWindowData;
static const sf::Color COLOR_GOODS(204, 255, 255);
static const sf::Color COLOR_SHOPSELECT(102, 178, 255);
static const sf::Color COLOR_BUYED(0, 0, 255, 100);
static const std::string shopI18N = "gui.basic.shop.default";

static bool isOpening = false;
static sf::RenderWindow window;
static sf::Vector2f windowSize;

static const float buyButtonEffectTime = 0.1;
static shop::Good* lastBuy = nullptr;
static long long lastBuyTick = 0;

bool gui::GuiShopDefault::canOpen()
{
    return renderer.isClosed();
}

gui::IGuiImplement* gui::GuiShopDefault::open()
{
    renderer.open();
    return &renderer;
}



void gui::GuiShopRenderer::render()
{
    if (isClosed())
        return;
    window.clear(COLOR_GOODS);
    sf::RectangleShape rect(sf::Vector2f(windowSize.x *0.2, windowSize.y));
    rect.setPosition({0,0});
    rect.setFillColor(COLOR_SHOPSELECT);
    window.draw(rect);

    int index = 0;
    sf::Vector2f posText( windowSize.x * 0.2 * 0.1, windowSize.y * 0.0625 * 0.5 );
    float downParam = windowSize.y * 0.0625;
    unsigned int fontSize = windowSize.y * 0.0625 * 0.8 * 0.75;
    for (auto& [name, shop] : shop::shops) {
        if (seleted == name) {
            gui::renderQuad(window, 
                sf::Vector2f( windowSize.x * 0.2 * 0.02, windowSize.y * 0.0625 * 0.02 + downParam * index ),
                sf::Vector2f( windowSize.x * 0.2 * 0.96, windowSize.y * 0.0625 * 0.96 ), sf::Color::White);
            gui::renderText(window, lang::get("shop." + name), posText + sf::Vector2f(0, downParam * index), gui::Left, fontSize, sf::Color::Black);
        }
        else {
            gui::renderText(window, lang::get("shop." + name), posText + sf::Vector2f(0,downParam*index), gui::Left, fontSize, sf::Color::White);
        }
        index++;
    }

    gui::renderText(window, lang::get("money") + std::to_string(save::saverDefault.getMoney()),
        sf::Vector2f(posText.x, posText.y + downParam * 15), gui::Left, fontSize);
    
    auto& shop = shop::shops[seleted];
    downParam = windowSize.y * 0.2;
    float rightParam = windowSize.x * 0.2;
    fontSize = windowSize.y * 0.2 * 0.15 * 0.75;
    auto minSize = min(windowSize.x, windowSize.y);
    sf::Vector2f iconSize(minSize, minSize);
    int i = 0, j = 0;
    for (auto& good : shop) {
        auto positionOrigin  = sf::Vector2f(windowSize.x * 0.2 + i * rightParam, j * downParam);
        renderText(window, good.getDisplayName(), positionOrigin + sf::Vector2f(windowSize.x * 0.2*0.5, windowSize.y * 0.2 * 0.075), gui::Center, fontSize);
        renderPicture(window,
            positionOrigin + sf::Vector2f(windowSize.x * 0.2 * 0.5, windowSize.y * 0.2 * (0.15 + 0.35)),
            sf::Vector2f(minSize * 0.2 * 0.7 * 0.9, minSize * 0.2 * 0.7 * 0.9), *good.getIcon());
        auto buyPosition = positionOrigin + sf::Vector2f(windowSize.x * 0.2 * 0.2, windowSize.y * 0.2 * 0.85);
        auto buySize = sf::Vector2f(windowSize.x * 0.2 * 0.6, windowSize.y * 0.2 * 0.15);
        renderQuad(window, buyPosition, buySize, gui::isOver(buyPosition, buySize, window)? gui::COLOR_OVEROPTION:sf::Color::White);
        renderText(window, 
            lang::get("money") + std::to_string(good.getPrice()), 
            buyPosition + sf::Vector2f(buySize.x * 0.5, buySize.y * 0.5), gui::Center, fontSize);
        if (lastBuy == &good) {
            auto passedTime = (tick::getTicks() + tick::getPartialTick() - lastBuyTick) * config::guideGlobal.tickMs / 1000;
            if (passedTime <= buyButtonEffectTime) 
                renderQuad(window, buyPosition, buySize, COLOR_BUYED);
        }
        
        i++;
        if (i >= 4) {
            i = 0;
            j++;
        }
    }

    window.display();
}

void gui::GuiShopRenderer::tick()
{
    if (isClosed())
        return;
    if (viewport::window.isOpen() && window.isOpen()) {
        
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                isOpening = false;
                return;
            }
            mouse::handleEvent(*event, shopDefaultWindowData);
        }
    }

    int index = 0;
    for (auto& [name, shop] : shop::shops) {
        auto position = sf::Vector2f(0, windowSize.y * 0.0625 * index);
        auto size = sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.0625);
        if (isOver(position, size, window) && mouse::isPressed(mouse::Left, 0, shopDefaultWindowData)) {
            //std::cout << name << std::endl;
            seleted = name;
            break;
        }
        index++;
    }

    auto& shop = shop::shops[seleted];
    float downParam = windowSize.y * 0.2;
    float rightParam = windowSize.x * 0.2;
    int i = 0, j = 0;
    for (auto& good : shop) {
        auto buyPosition = sf::Vector2f(windowSize.x * 0.2 + i * rightParam, j * downParam) + sf::Vector2f(windowSize.x * 0.2 * 0.2, windowSize.y * 0.2 * 0.85);
        auto buySize = sf::Vector2f(windowSize.x * 0.2 * 0.6, windowSize.y * 0.2 * 0.15);
        if (gui::isOver(buyPosition, buySize, window) && mouse::isPressed(mouse::Left, 0, shopDefaultWindowData)) {
            if (save::saverDefault.cost(good.getPrice())) {
                good.buy();
                lastBuy = &good;
                lastBuyTick = tick::getTicks();
            }
        }
        i++;
        if (i >= 4) {
            i = 0;
            j++;
        }
    }
}

bool gui::GuiShopRenderer::isClosed()
{
    return !isOpening;
}

void gui::GuiShopRenderer::open()
{
    windowSize = sf::Vector2f( config::screenWidth * 0.5, config::screenHeight * 0.5 );
    window = sf::RenderWindow(sf::VideoMode(sf::Vector2u(windowSize.x, windowSize.y)), util::convert(lang::get(shopI18N)), sf::Style::Close);
    auto handle = window.getNativeHandle();
    //SetParent(handle, viewport::window.getNativeHandle());
    SetWindowPos(handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
    SetWindowLong(handle, GWL_EXSTYLE, (GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED));
    window.setIcon(asset::getTexture(file::getPath({ config::guideGlobal.shopIcon }))->copyToImage());
    window.clear(COLOR_GOODS);
    isOpening = true;
}