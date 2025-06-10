#pragma once

#include "trans.h"
#include "ani.h"

namespace trans {
    class ConditionPlayEnd : public ICondition {
    public:
        bool evaluate() override {
            return !((ani::Animation*)ani::animationController.getCurrentState())->isPlaying();
        }
        void loadJson(nlohmann::json& conditionData) override {
        }
    };
    class ConditionPlayEndFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "PlayEnd";
        }
        ICondition* newCondition() override {
            return new ConditionPlayEnd();
        }
    };
}
