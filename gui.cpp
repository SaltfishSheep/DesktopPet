#include "gui.h"
#include "viewport.h"
#include "config.h"
#include "mouse.h"
#include "file.h"
#include "util.h"

#include "gui_shop_default.h"
#include "gui_money.hpp"
#include "gui_exit.hpp"

#include <SFML\Graphics.hpp>
#include <iostream>

static sf::RenderWindow windowGui;
static std::vector<int> size;

void gui::init() {
    shop.addSub(new GuiNode( "default", "gui.basic.shop.default", new GuiShopDefault()));

    interact.addSub(new GuiNode( "money", "gui.basic.interact.money", new GuiMoney()));

    system.addSub(new GuiNode( "exit", "gui.basic.system.exit", new GuiExit()));
}

gui::GuiOption::GuiOption(std::string name, std::string keyI18N) : name(name), ObjectI18N(keyI18N)
{
}

void gui::GuiOption::addSub(gui::GuiNode* node)
{
	subs.push_back(node);
}

gui::GuiNode* gui::GuiOption::getSub(int index)
{
    return subs[index];
}

int gui::GuiOption::getSubCount()
{
    return subs.size();
}

gui::GuiOption::~GuiOption()
{
    for (auto& sub : subs)
        delete sub;
}

gui::GuiNode::GuiNode(std::string name, std::string keyI18N, IGui* gui) : name(name), ObjectI18N(keyI18N), gui(gui)
{
}

gui::GuiNode::~GuiNode()
{
	delete gui;
}

gui::IGuiImplement* gui::GuiNode::open()
{
	if (this->gui->canOpen())
        return this->gui->open();
    return nullptr;
}

static bool isDisplaying = false;
static int displaying = -1;

bool gui::isOver(sf::Vector2f position, sf::Vector2f size, sf::RenderWindow& reactWindow)
{
    auto mouse = sf::Mouse::getPosition(reactWindow);
    return mouse.x >= position.x && mouse.y >= position.y &&
        mouse.x < position.x + size.x && mouse.y < position.y + size.y;
}

static int isOverOption() {
    auto& windowSize = config::guideGlobal.windowSize;
    for (int i = 0; i < 4; i++) {
        if (gui::isOver(
            sf::Vector2f( windowSize[0] * 0.25 * i , windowSize[1] * 0.8 ),
            sf::Vector2f( windowSize[0] * 0.25, windowSize[1] * 0.2 )))
            return i;
    }
    return -1;
}

gui::GuiController::GuiController()
{
    this->renderLayer = 2;
}

void gui::GuiController::tick()
{
    auto iter = implements.begin();
    while (iter != implements.end())
    {
        if ((*iter)->isClosed()) {
            iter = implements.erase(iter);
            continue;
        }
        (*iter)->tick();
        iter++;
    }

    if (mouse::isReleased(mouse::Right)) {
        isDisplaying = !isDisplaying;
        return;
    }

    if (!isDisplaying) {
        displaying = -1;
        return;
    }

    int overOption = isOverOption();

    if (overOption < 0 &&  displaying < 0)
        return;

    if (mouse::isPressed(mouse::Left)) {
        
        if (overOption >= 0) {
            if (displaying == overOption)
                displaying = -1;
            else
                displaying = overOption;
        }
        else if (displaying >= 0 && displaying != 1) {
            auto& windowSize = config::guideGlobal.windowSize;
            auto subCount = options[displaying]->getSubCount();
            sf::Vector2f positionFirst = sf::Vector2f(windowSize[0] * 0.25 * displaying, windowSize[1] * 0.7);
            auto upParam = windowSize[1] * 0.1;
            sf::Vector2f size = sf::Vector2f(windowSize[0] * 0.45, windowSize[1] * 0.1);
            for (int i = 0; i < subCount; i++) {
                if (isOver(positionFirst + sf::Vector2f(0, i * upParam), size)) {
                    auto openGui = options[displaying]->getSub(i)->open();
                    if (openGui != nullptr)
                        this->addGuiImplement(openGui);
                }
            }
        }
    }

}

void gui::GuiController::render()
{
    
    auto iter = implements.begin();
    while (iter != implements.end())
    { 
        if ((*iter)->isClosed()) {
            iter = implements.erase(iter);
            continue;
        }
        if ((*iter)->renderLayer >= this->renderLayer)
            break;
        (*iter)->render();
        iter++;
    }

    if (isDisplaying) {
        auto& windowSize = config::guideGlobal.windowSize;

        for (int i = 0; i < 4; i++)
        {
            auto color = isOverOption() == i ? COLOR_OVEROPTION : sf::Color::White;
            renderQuad(viewport::window, sf::Vector2f(windowSize[0] * 0.25 * i, windowSize[1] * 0.8), sf::Vector2f(windowSize[0] * 0.25, windowSize[1] * 0.2), color);
        }

        sf::VertexArray line(sf::PrimitiveType::Lines, 6);
        line[0].position = sf::Vector2f(windowSize[0] * 0.25, windowSize[1] * 0.82);
        line[1].position = sf::Vector2f(windowSize[0] * 0.25, windowSize[1] * 0.98);
        line[2].position = sf::Vector2f(windowSize[0] * 0.50, windowSize[1] * 0.82);
        line[3].position = sf::Vector2f(windowSize[0] * 0.50, windowSize[1] * 0.98);
        line[4].position = sf::Vector2f(windowSize[0] * 0.75, windowSize[1] * 0.82);
        line[5].position = sf::Vector2f(windowSize[0] * 0.75, windowSize[1] * 0.98);
        for (int i = 0; i < 6; i++)
            line[i].color = COLOR_ASBLACK;

        viewport::window.draw(line);

        for (int i = 0; i < 4; i++) {
            sf::Text text(config::font, util::convert(gui::options[i]->getDisplayName()), (unsigned int)(windowSize[1] * 0.2 * 0.4 * 0.75));
            text.setFillColor(COLOR_ASBLACK);
            renderText(viewport::window, text,
                sf::Vector2f(windowSize[0] * 0.25 * (i + 0.5), windowSize[1] * 0.9));
        }



        if (displaying >= 0) {

            renderQuad(viewport::window, sf::Vector2f(windowSize[0] * 0.25 * displaying, windowSize[1] * 0.8), sf::Vector2f(windowSize[0] * 0.25, windowSize[1] * 0.2), COLOR_DISPLAYING);

            if (displaying != 1) {
                auto subCount = options[displaying]->getSubCount();
                sf::Vector2f positionFirst = sf::Vector2f(windowSize[0] * 0.25 * displaying, windowSize[1] * 0.7);
                auto upParam = windowSize[1] * 0.1;
                sf::Vector2f size = sf::Vector2f(windowSize[0] * 0.45, windowSize[1] * 0.1);
                for (int i = 0; i < subCount; i++) {
                    auto position = positionFirst + sf::Vector2f(0, upParam * i);
                    renderQuad(viewport::window, position, size, isOver(position, size) ? COLOR_OVEROPTION : sf::Color::White);
                    sf::Text text(config::font, util::convert(options[displaying]->getSub(i)->getDisplayName()), (unsigned int)(windowSize[1] * 0.2 * 0.4 * 0.75));
                    text.setFillColor(COLOR_ASBLACK);
                    renderText(viewport::window, text, position + sf::Vector2f(size.x * 0.5, size.y * 0.5), Center);
                }
            } else {
                auto position = sf::Vector2f(windowSize[0] * 0.1, windowSize[1] * 0.3);
                auto size = sf::Vector2f(windowSize[0] * 0.8, windowSize[1] * 0.5);
                renderQuad(viewport::window, position, size, sf::Color::White);
                renderLine(viewport::window, { position.x, position.y + size.y }, { position.x + size.x, position.y + size.y }, COLOR_ASBLACK);
                auto textPosition = position + sf::Vector2f(size.x * 0.1, size.y * 0.1);
                unsigned int fontSize = windowSize[1] * 0.5 * 0.2 * 0.8 * 0.75;
                renderText(viewport::window, lang::get("gui.basic.state.money") + std::to_string(save::saverDefault.getMoney()), textPosition, Left, fontSize);
            }
        }
    }

    while (iter != implements.end())
    {
        if ((*iter)->isClosed()) {
            iter = implements.erase(iter);
            continue;
        }
        (*iter)->render();
        iter++;
    }

}

void gui::GuiController::addGuiImplement(IGuiImplement* implement)
{
    for (auto implementIn : implements)
        if (implementIn == implement)
            return;
    std::list<IGuiImplement*> tempList;
    while (!implements.empty()) {
        IGuiImplement* endOne = implements.back();
        if (endOne->renderLayer <= implement->renderLayer)
            break;
        implements.pop_back();
        tempList.push_back(endOne);
    }
    implements.push_back(implement);
    while (!tempList.empty()) {
        IGuiImplement* endOne = tempList.back();
        tempList.pop_back();
        implements.push_back(endOne);
    }
}

void gui::renderText(sf::RenderWindow& renderWindow, sf::Text text, sf::Vector2f position, TextAlign align)
{
    switch (align) {
    case Center:
        text.setPosition({ position.x - text.getLocalBounds().size.x / 2, position.y - text.getLocalBounds().size.y / 2 });
        break;
    case Left:
        text.setPosition({ position.x, position.y - text.getLocalBounds().size.y / 2 });
        break;
    case Right:
        text.setPosition({ position.x - text.getLocalBounds().size.x, position.y - text.getLocalBounds().size.y / 2 });
        break;
    }
    renderWindow.draw(text);
}

void gui::renderText(sf::RenderWindow& renderWindow, std::string text, sf::Vector2f position, TextAlign align, unsigned int fontSize, sf::Color fontColor)
{
    sf::Text textsf(config::font, util::convert(text), fontSize);
    textsf.setFillColor(fontColor);
    renderText(renderWindow,textsf, position, align);
}

void gui::renderLine(sf::RenderWindow& renderWindow, sf::Vector2f start, sf::Vector2f end, sf::Color color)
{
    sf::VertexArray line(sf::PrimitiveType::Lines, 2);
    line[0].position = start;
    line[0].color = color;
    line[1].position = end;
    line[1].color = color;
    renderWindow.draw(line);
}

void gui::renderQuad(sf::RenderWindow& renderWindow, sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
    sf::RectangleShape shape(size);
    shape.setFillColor(color);
    shape.setPosition(position);
    renderWindow.draw(shape);
}

void gui::renderPicture(sf::RenderWindow& renderWindow, sf::Vector2f position, sf::Vector2f size, sf::Texture& texture)
{
    sf::Sprite sprite(texture);
    sprite.setPosition(position - sf::Vector2f(size.x * 0.5, size.y * 0.5));
    auto textureSize = texture.getSize();
    sprite.setScale(sf::Vector2f(size.x/textureSize.x, size.y/textureSize.y));
    renderWindow.draw(sprite);
}
