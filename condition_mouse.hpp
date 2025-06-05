#pragma once

#include "trans.h"
#include "mouse.h"
#include "viewport.h"
#include "config.h"

#include <iostream>

namespace trans {

    inline bool (*positionEvaluate[4])();
    inline bool (*eventEvaluate[10])();

    class ConditionMouse : public trans::ICondition {
    public:
        bool evaluate() override {
            //std::cout << "Mouse" << std::endl;
            if (!isFunctionsInited) {
                positionEvaluate[0] = []()->bool {
                    sf::Vector2i globalPosition = sf::Mouse::getPosition(viewport::window);
                    return globalPosition.y >= 0 &&
                        globalPosition.y <= config::guideGlobal.windowSize[1] / 3 &&
                        globalPosition.x >= 0 &&
                        globalPosition.x <= config::guideGlobal.windowSize[0];
                };
                positionEvaluate[1] = []()->bool {
                    sf::Vector2i globalPosition = sf::Mouse::getPosition(viewport::window);
                    return globalPosition.y >= config::guideGlobal.windowSize[1] / 3 &&
                        globalPosition.y <= config::guideGlobal.windowSize[1] /3 * 2 &&
                        globalPosition.x >= 0 &&
                        globalPosition.x <= config::guideGlobal.windowSize[0];
                };
                positionEvaluate[2] = []()->bool {
                    sf::Vector2i globalPosition = sf::Mouse::getPosition(viewport::window);
                    return globalPosition.y >= config::guideGlobal.windowSize[1] / 3 * 2 &&
                        globalPosition.y <= config::guideGlobal.windowSize[1] / 3 * 3 &&
                        globalPosition.x >= 0 &&
                        globalPosition.x <= config::guideGlobal.windowSize[0];
                };
                positionEvaluate[3] = []()->bool {
                    return true;
                };
                eventEvaluate[0] = []()->bool {
                    return mouse::isPressed(mouse::MouseButton::Left);
                };
                eventEvaluate[1] = []()->bool {
                    return mouse::isReleased(mouse::MouseButton::Left);
                };
                eventEvaluate[2] = []()->bool {
                    return mouse::isClick(mouse::MouseButton::Left);
                };
                eventEvaluate[3] = []()->bool {
                    return mouse::isLongPressed(mouse::MouseButton::Left);
                };
                eventEvaluate[4] = []()->bool {
                    return mouse::isPressed(mouse::MouseButton::Middle);
                };
                eventEvaluate[5] = []()->bool {
                    return mouse::isReleased(mouse::MouseButton::Middle);
                };
                eventEvaluate[6] = []()->bool {
                    return mouse::isClick(mouse::MouseButton::Middle);
                };
                eventEvaluate[7] = []()->bool {
                    return mouse::isLongPressed(mouse::MouseButton::Middle);
                };
                eventEvaluate[8] = []()->bool {
                    return mouse::getWheelState() > 0;
                };
                eventEvaluate[9] = []()->bool {
                    return mouse::getWheelState() < 0;
                };
                isFunctionsInited = true;
            }

            bool isInPosition = false;
            for (int i = 0; i < 3; i++) {
                if (positionFlag[i] && positionEvaluate[i]()) {
                    isInPosition = true;
                    break;
                }
            }
            if (!isInPosition)
                return false;
            for (int i = 0; i < 10; i++)
                if (eventFlag[i] && eventEvaluate[i]())
                    return true;
            return false;
        }
        void loadJson(nlohmann::json & conditionData) override {
            if (conditionData.contains("position")) {
                auto& position = conditionData["position"];
                for (auto& each : position)
                    if (each.get<std::string>() == "up")
                        positionFlag[0] = true;
                    else if (each.get<std::string>() == "middle")
                        positionFlag[1] = true;
                    else if (each.get<std::string>() == "down")
                        positionFlag[2] = true;
                    else if (each.get<std::string>() == "any")
                        positionFlag[3] = true;
            } else
                positionFlag[3] = true;
            
            if (conditionData.contains("event")) {
                auto& event = conditionData["event"];
                for (auto& each : event)
                    if (each.get<std::string>() == "pressed")
                        eventFlag[0] = true;
                    else  if (each.get<std::string>() == "released")
                        eventFlag[1] = true;
                    else if (each.get<std::string>() == "click")
                        eventFlag[2] = true;
                    else if (each.get<std::string>() == "long_pressed")
                        eventFlag[3] = true;
                    else if (each.get<std::string>() == "wheel_pressed")
                        eventFlag[4] = true;
                    else if (each.get<std::string>() == "wheel_released")
                        eventFlag[5] = true;
                    else if (each.get<std::string>() == "wheel_click")
                        eventFlag[6] = true;
                    else if (each.get<std::string>() == "wheel_long_pressing")
                        eventFlag[7] = true;
                    else if (each.get<std::string>() == "wheel_up")
                        eventFlag[8] = true;
                    else if (each.get<std::string>() == "wheel_down")
                        eventFlag[9] = true;
            }
            
        }
    protected:
        bool positionFlag[4] = { false, false, false, false };
        bool eventFlag[10] = { false, false, false, false, false, false, false, false, false,  false };
    private:
        static bool isFunctionsInited;
    };
    bool ConditionMouse::isFunctionsInited = false;
    class ConditionMouseFactory : public trans::IConditionFactory {
    public:
        virtual trans::ICondition* newCondition() override {
            return new ConditionMouse();
        }
        std::string type() override {
            return "Mouse";
        }
    };

}
